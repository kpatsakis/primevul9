static int i40e_up_complete(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	int err;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED)
		i40e_vsi_configure_msix(vsi);
	else
		i40e_configure_msi_and_legacy(vsi);

	/* start rings */
	err = i40e_vsi_start_rings(vsi);
	if (err)
		return err;

	clear_bit(__I40E_VSI_DOWN, vsi->state);
	i40e_napi_enable_all(vsi);
	i40e_vsi_enable_irq(vsi);

	if ((pf->hw.phy.link_info.link_info & I40E_AQ_LINK_UP) &&
	    (vsi->netdev)) {
		i40e_print_link_message(vsi, true);
		netif_tx_start_all_queues(vsi->netdev);
		netif_carrier_on(vsi->netdev);
	}

	/* replay FDIR SB filters */
	if (vsi->type == I40E_VSI_FDIR) {
		/* reset fd counters */
		pf->fd_add_err = 0;
		pf->fd_atr_cnt = 0;
		i40e_fdir_filter_restore(vsi);
	}

	/* On the next run of the service_task, notify any clients of the new
	 * opened netdev
	 */
	set_bit(__I40E_CLIENT_SERVICE_REQUESTED, pf->state);
	i40e_service_event_schedule(pf);

	return 0;
}