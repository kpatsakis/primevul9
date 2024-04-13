static struct i40e_vsi *i40e_vsi_reinit_setup(struct i40e_vsi *vsi)
{
	u16 alloc_queue_pairs;
	struct i40e_pf *pf;
	u8 enabled_tc;
	int ret;

	if (!vsi)
		return NULL;

	pf = vsi->back;

	i40e_put_lump(pf->qp_pile, vsi->base_queue, vsi->idx);
	i40e_vsi_clear_rings(vsi);

	i40e_vsi_free_arrays(vsi, false);
	i40e_set_num_rings_in_vsi(vsi);
	ret = i40e_vsi_alloc_arrays(vsi, false);
	if (ret)
		goto err_vsi;

	alloc_queue_pairs = vsi->alloc_queue_pairs *
			    (i40e_enabled_xdp_vsi(vsi) ? 2 : 1);

	ret = i40e_get_lump(pf, pf->qp_pile, alloc_queue_pairs, vsi->idx);
	if (ret < 0) {
		dev_info(&pf->pdev->dev,
			 "failed to get tracking for %d queues for VSI %d err %d\n",
			 alloc_queue_pairs, vsi->seid, ret);
		goto err_vsi;
	}
	vsi->base_queue = ret;

	/* Update the FW view of the VSI. Force a reset of TC and queue
	 * layout configurations.
	 */
	enabled_tc = pf->vsi[pf->lan_vsi]->tc_config.enabled_tc;
	pf->vsi[pf->lan_vsi]->tc_config.enabled_tc = 0;
	pf->vsi[pf->lan_vsi]->seid = pf->main_vsi_seid;
	i40e_vsi_config_tc(pf->vsi[pf->lan_vsi], enabled_tc);
	if (vsi->type == I40E_VSI_MAIN)
		i40e_rm_default_mac_filter(vsi, pf->hw.mac.perm_addr);

	/* assign it some queues */
	ret = i40e_alloc_rings(vsi);
	if (ret)
		goto err_rings;

	/* map all of the rings to the q_vectors */
	i40e_vsi_map_rings_to_vectors(vsi);
	return vsi;

err_rings:
	i40e_vsi_free_q_vectors(vsi);
	if (vsi->netdev_registered) {
		vsi->netdev_registered = false;
		unregister_netdev(vsi->netdev);
		free_netdev(vsi->netdev);
		vsi->netdev = NULL;
	}
	i40e_aq_delete_element(&pf->hw, vsi->seid, NULL);
err_vsi:
	i40e_vsi_clear(vsi);
	return NULL;
}