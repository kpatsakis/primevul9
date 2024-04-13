static void i40e_link_event(struct i40e_pf *pf)
{
	struct i40e_vsi *vsi = pf->vsi[pf->lan_vsi];
	u8 new_link_speed, old_link_speed;
	i40e_status status;
	bool new_link, old_link;

	/* set this to force the get_link_status call to refresh state */
	pf->hw.phy.get_link_info = true;
	old_link = (pf->hw.phy.link_info_old.link_info & I40E_AQ_LINK_UP);
	status = i40e_get_link_status(&pf->hw, &new_link);

	/* On success, disable temp link polling */
	if (status == I40E_SUCCESS) {
		clear_bit(__I40E_TEMP_LINK_POLLING, pf->state);
	} else {
		/* Enable link polling temporarily until i40e_get_link_status
		 * returns I40E_SUCCESS
		 */
		set_bit(__I40E_TEMP_LINK_POLLING, pf->state);
		dev_dbg(&pf->pdev->dev, "couldn't get link state, status: %d\n",
			status);
		return;
	}

	old_link_speed = pf->hw.phy.link_info_old.link_speed;
	new_link_speed = pf->hw.phy.link_info.link_speed;

	if (new_link == old_link &&
	    new_link_speed == old_link_speed &&
	    (test_bit(__I40E_VSI_DOWN, vsi->state) ||
	     new_link == netif_carrier_ok(vsi->netdev)))
		return;

	i40e_print_link_message(vsi, new_link);

	/* Notify the base of the switch tree connected to
	 * the link.  Floating VEBs are not notified.
	 */
	if (pf->lan_veb < I40E_MAX_VEB && pf->veb[pf->lan_veb])
		i40e_veb_link_event(pf->veb[pf->lan_veb], new_link);
	else
		i40e_vsi_link_event(vsi, new_link);

	if (pf->vf)
		i40e_vc_notify_link_state(pf);

	if (pf->flags & I40E_FLAG_PTP)
		i40e_ptp_set_increment(pf);
}