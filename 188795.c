struct i40e_vsi *i40e_vsi_setup(struct i40e_pf *pf, u8 type,
				u16 uplink_seid, u32 param1)
{
	struct i40e_vsi *vsi = NULL;
	struct i40e_veb *veb = NULL;
	u16 alloc_queue_pairs;
	int ret, i;
	int v_idx;

	/* The requested uplink_seid must be either
	 *     - the PF's port seid
	 *              no VEB is needed because this is the PF
	 *              or this is a Flow Director special case VSI
	 *     - seid of an existing VEB
	 *     - seid of a VSI that owns an existing VEB
	 *     - seid of a VSI that doesn't own a VEB
	 *              a new VEB is created and the VSI becomes the owner
	 *     - seid of the PF VSI, which is what creates the first VEB
	 *              this is a special case of the previous
	 *
	 * Find which uplink_seid we were given and create a new VEB if needed
	 */
	for (i = 0; i < I40E_MAX_VEB; i++) {
		if (pf->veb[i] && pf->veb[i]->seid == uplink_seid) {
			veb = pf->veb[i];
			break;
		}
	}

	if (!veb && uplink_seid != pf->mac_seid) {

		for (i = 0; i < pf->num_alloc_vsi; i++) {
			if (pf->vsi[i] && pf->vsi[i]->seid == uplink_seid) {
				vsi = pf->vsi[i];
				break;
			}
		}
		if (!vsi) {
			dev_info(&pf->pdev->dev, "no such uplink_seid %d\n",
				 uplink_seid);
			return NULL;
		}

		if (vsi->uplink_seid == pf->mac_seid)
			veb = i40e_veb_setup(pf, 0, pf->mac_seid, vsi->seid,
					     vsi->tc_config.enabled_tc);
		else if ((vsi->flags & I40E_VSI_FLAG_VEB_OWNER) == 0)
			veb = i40e_veb_setup(pf, 0, vsi->uplink_seid, vsi->seid,
					     vsi->tc_config.enabled_tc);
		if (veb) {
			if (vsi->seid != pf->vsi[pf->lan_vsi]->seid) {
				dev_info(&vsi->back->pdev->dev,
					 "New VSI creation error, uplink seid of LAN VSI expected.\n");
				return NULL;
			}
			/* We come up by default in VEPA mode if SRIOV is not
			 * already enabled, in which case we can't force VEPA
			 * mode.
			 */
			if (!(pf->flags & I40E_FLAG_VEB_MODE_ENABLED)) {
				veb->bridge_mode = BRIDGE_MODE_VEPA;
				pf->flags &= ~I40E_FLAG_VEB_MODE_ENABLED;
			}
			i40e_config_bridge_mode(veb);
		}
		for (i = 0; i < I40E_MAX_VEB && !veb; i++) {
			if (pf->veb[i] && pf->veb[i]->seid == vsi->uplink_seid)
				veb = pf->veb[i];
		}
		if (!veb) {
			dev_info(&pf->pdev->dev, "couldn't add VEB\n");
			return NULL;
		}

		vsi->flags |= I40E_VSI_FLAG_VEB_OWNER;
		uplink_seid = veb->seid;
	}

	/* get vsi sw struct */
	v_idx = i40e_vsi_mem_alloc(pf, type);
	if (v_idx < 0)
		goto err_alloc;
	vsi = pf->vsi[v_idx];
	if (!vsi)
		goto err_alloc;
	vsi->type = type;
	vsi->veb_idx = (veb ? veb->idx : I40E_NO_VEB);

	if (type == I40E_VSI_MAIN)
		pf->lan_vsi = v_idx;
	else if (type == I40E_VSI_SRIOV)
		vsi->vf_id = param1;
	/* assign it some queues */
	alloc_queue_pairs = vsi->alloc_queue_pairs *
			    (i40e_enabled_xdp_vsi(vsi) ? 2 : 1);

	ret = i40e_get_lump(pf, pf->qp_pile, alloc_queue_pairs, vsi->idx);
	if (ret < 0) {
		dev_info(&pf->pdev->dev,
			 "failed to get tracking for %d queues for VSI %d err=%d\n",
			 alloc_queue_pairs, vsi->seid, ret);
		goto err_vsi;
	}
	vsi->base_queue = ret;

	/* get a VSI from the hardware */
	vsi->uplink_seid = uplink_seid;
	ret = i40e_add_vsi(vsi);
	if (ret)
		goto err_vsi;

	switch (vsi->type) {
	/* setup the netdev if needed */
	case I40E_VSI_MAIN:
	case I40E_VSI_VMDQ2:
		ret = i40e_config_netdev(vsi);
		if (ret)
			goto err_netdev;
		ret = register_netdev(vsi->netdev);
		if (ret)
			goto err_netdev;
		vsi->netdev_registered = true;
		netif_carrier_off(vsi->netdev);
#ifdef CONFIG_I40E_DCB
		/* Setup DCB netlink interface */
		i40e_dcbnl_setup(vsi);
#endif /* CONFIG_I40E_DCB */
		/* fall through */

	case I40E_VSI_FDIR:
		/* set up vectors and rings if needed */
		ret = i40e_vsi_setup_vectors(vsi);
		if (ret)
			goto err_msix;

		ret = i40e_alloc_rings(vsi);
		if (ret)
			goto err_rings;

		/* map all of the rings to the q_vectors */
		i40e_vsi_map_rings_to_vectors(vsi);

		i40e_vsi_reset_stats(vsi);
		break;

	default:
		/* no netdev or rings for the other VSI types */
		break;
	}

	if ((pf->hw_features & I40E_HW_RSS_AQ_CAPABLE) &&
	    (vsi->type == I40E_VSI_VMDQ2)) {
		ret = i40e_vsi_config_rss(vsi);
	}
	return vsi;

err_rings:
	i40e_vsi_free_q_vectors(vsi);
err_msix:
	if (vsi->netdev_registered) {
		vsi->netdev_registered = false;
		unregister_netdev(vsi->netdev);
		free_netdev(vsi->netdev);
		vsi->netdev = NULL;
	}
err_netdev:
	i40e_aq_delete_element(&pf->hw, vsi->seid, NULL);
err_vsi:
	i40e_vsi_clear(vsi);
err_alloc:
	return NULL;
}