static void i40e_vsi_config_netdev_tc(struct i40e_vsi *vsi, u8 enabled_tc)
{
	struct net_device *netdev = vsi->netdev;
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	u8 netdev_tc = 0;
	int i;
	struct i40e_dcbx_config *dcbcfg = &hw->local_dcbx_config;

	if (!netdev)
		return;

	if (!enabled_tc) {
		netdev_reset_tc(netdev);
		return;
	}

	/* Set up actual enabled TCs on the VSI */
	if (netdev_set_num_tc(netdev, vsi->tc_config.numtc))
		return;

	/* set per TC queues for the VSI */
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		/* Only set TC queues for enabled tcs
		 *
		 * e.g. For a VSI that has TC0 and TC3 enabled the
		 * enabled_tc bitmap would be 0x00001001; the driver
		 * will set the numtc for netdev as 2 that will be
		 * referenced by the netdev layer as TC 0 and 1.
		 */
		if (vsi->tc_config.enabled_tc & BIT(i))
			netdev_set_tc_queue(netdev,
					vsi->tc_config.tc_info[i].netdev_tc,
					vsi->tc_config.tc_info[i].qcount,
					vsi->tc_config.tc_info[i].qoffset);
	}

	if (pf->flags & I40E_FLAG_TC_MQPRIO)
		return;

	/* Assign UP2TC map for the VSI */
	for (i = 0; i < I40E_MAX_USER_PRIORITY; i++) {
		/* Get the actual TC# for the UP */
		u8 ets_tc = dcbcfg->etscfg.prioritytable[i];
		/* Get the mapped netdev TC# for the UP */
		netdev_tc =  vsi->tc_config.tc_info[ets_tc].netdev_tc;
		netdev_set_prio_tc_map(netdev, i, netdev_tc);
	}
}