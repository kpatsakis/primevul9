int i40e_vsi_release(struct i40e_vsi *vsi)
{
	struct i40e_mac_filter *f;
	struct hlist_node *h;
	struct i40e_veb *veb = NULL;
	struct i40e_pf *pf;
	u16 uplink_seid;
	int i, n, bkt;

	pf = vsi->back;

	/* release of a VEB-owner or last VSI is not allowed */
	if (vsi->flags & I40E_VSI_FLAG_VEB_OWNER) {
		dev_info(&pf->pdev->dev, "VSI %d has existing VEB %d\n",
			 vsi->seid, vsi->uplink_seid);
		return -ENODEV;
	}
	if (vsi == pf->vsi[pf->lan_vsi] &&
	    !test_bit(__I40E_DOWN, pf->state)) {
		dev_info(&pf->pdev->dev, "Can't remove PF VSI\n");
		return -ENODEV;
	}

	uplink_seid = vsi->uplink_seid;
	if (vsi->type != I40E_VSI_SRIOV) {
		if (vsi->netdev_registered) {
			vsi->netdev_registered = false;
			if (vsi->netdev) {
				/* results in a call to i40e_close() */
				unregister_netdev(vsi->netdev);
			}
		} else {
			i40e_vsi_close(vsi);
		}
		i40e_vsi_disable_irq(vsi);
	}

	spin_lock_bh(&vsi->mac_filter_hash_lock);

	/* clear the sync flag on all filters */
	if (vsi->netdev) {
		__dev_uc_unsync(vsi->netdev, NULL);
		__dev_mc_unsync(vsi->netdev, NULL);
	}

	/* make sure any remaining filters are marked for deletion */
	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist)
		__i40e_del_filter(vsi, f);

	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	i40e_sync_vsi_filters(vsi);

	i40e_vsi_delete(vsi);
	i40e_vsi_free_q_vectors(vsi);
	if (vsi->netdev) {
		free_netdev(vsi->netdev);
		vsi->netdev = NULL;
	}
	i40e_vsi_clear_rings(vsi);
	i40e_vsi_clear(vsi);

	/* If this was the last thing on the VEB, except for the
	 * controlling VSI, remove the VEB, which puts the controlling
	 * VSI onto the next level down in the switch.
	 *
	 * Well, okay, there's one more exception here: don't remove
	 * the orphan VEBs yet.  We'll wait for an explicit remove request
	 * from up the network stack.
	 */
	for (n = 0, i = 0; i < pf->num_alloc_vsi; i++) {
		if (pf->vsi[i] &&
		    pf->vsi[i]->uplink_seid == uplink_seid &&
		    (pf->vsi[i]->flags & I40E_VSI_FLAG_VEB_OWNER) == 0) {
			n++;      /* count the VSIs */
		}
	}
	for (i = 0; i < I40E_MAX_VEB; i++) {
		if (!pf->veb[i])
			continue;
		if (pf->veb[i]->uplink_seid == uplink_seid)
			n++;     /* count the VEBs */
		if (pf->veb[i]->seid == uplink_seid)
			veb = pf->veb[i];
	}
	if (n == 0 && veb && veb->uplink_seid != 0)
		i40e_veb_release(veb);

	return 0;
}