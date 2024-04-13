static int i40e_setup_pf_switch(struct i40e_pf *pf, bool reinit)
{
	u16 flags = 0;
	int ret;

	/* find out what's out there already */
	ret = i40e_fetch_switch_configuration(pf, false);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "couldn't fetch switch config, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		return ret;
	}
	i40e_pf_reset_stats(pf);

	/* set the switch config bit for the whole device to
	 * support limited promisc or true promisc
	 * when user requests promisc. The default is limited
	 * promisc.
	*/

	if ((pf->hw.pf_id == 0) &&
	    !(pf->flags & I40E_FLAG_TRUE_PROMISC_SUPPORT)) {
		flags = I40E_AQ_SET_SWITCH_CFG_PROMISC;
		pf->last_sw_conf_flags = flags;
	}

	if (pf->hw.pf_id == 0) {
		u16 valid_flags;

		valid_flags = I40E_AQ_SET_SWITCH_CFG_PROMISC;
		ret = i40e_aq_set_switch_config(&pf->hw, flags, valid_flags, 0,
						NULL);
		if (ret && pf->hw.aq.asq_last_status != I40E_AQ_RC_ESRCH) {
			dev_info(&pf->pdev->dev,
				 "couldn't set switch config bits, err %s aq_err %s\n",
				 i40e_stat_str(&pf->hw, ret),
				 i40e_aq_str(&pf->hw,
					     pf->hw.aq.asq_last_status));
			/* not a fatal problem, just keep going */
		}
		pf->last_sw_conf_valid_flags = valid_flags;
	}

	/* first time setup */
	if (pf->lan_vsi == I40E_NO_VSI || reinit) {
		struct i40e_vsi *vsi = NULL;
		u16 uplink_seid;

		/* Set up the PF VSI associated with the PF's main VSI
		 * that is already in the HW switch
		 */
		if (pf->lan_veb < I40E_MAX_VEB && pf->veb[pf->lan_veb])
			uplink_seid = pf->veb[pf->lan_veb]->seid;
		else
			uplink_seid = pf->mac_seid;
		if (pf->lan_vsi == I40E_NO_VSI)
			vsi = i40e_vsi_setup(pf, I40E_VSI_MAIN, uplink_seid, 0);
		else if (reinit)
			vsi = i40e_vsi_reinit_setup(pf->vsi[pf->lan_vsi]);
		if (!vsi) {
			dev_info(&pf->pdev->dev, "setup of MAIN VSI failed\n");
			i40e_cloud_filter_exit(pf);
			i40e_fdir_teardown(pf);
			return -EAGAIN;
		}
	} else {
		/* force a reset of TC and queue layout configurations */
		u8 enabled_tc = pf->vsi[pf->lan_vsi]->tc_config.enabled_tc;

		pf->vsi[pf->lan_vsi]->tc_config.enabled_tc = 0;
		pf->vsi[pf->lan_vsi]->seid = pf->main_vsi_seid;
		i40e_vsi_config_tc(pf->vsi[pf->lan_vsi], enabled_tc);
	}
	i40e_vlan_stripping_disable(pf->vsi[pf->lan_vsi]);

	i40e_fdir_sb_setup(pf);

	/* Setup static PF queue filter control settings */
	ret = i40e_setup_pf_filter_control(pf);
	if (ret) {
		dev_info(&pf->pdev->dev, "setup_pf_filter_control failed: %d\n",
			 ret);
		/* Failure here should not stop continuing other steps */
	}

	/* enable RSS in the HW, even for only one queue, as the stack can use
	 * the hash
	 */
	if ((pf->flags & I40E_FLAG_RSS_ENABLED))
		i40e_pf_config_rss(pf);

	/* fill in link information and enable LSE reporting */
	i40e_link_event(pf);

	/* Initialize user-specific link properties */
	pf->fc_autoneg_status = ((pf->hw.phy.link_info.an_info &
				  I40E_AQ_AN_COMPLETED) ? true : false);

	i40e_ptp_init(pf);

	/* repopulate tunnel port filters */
	i40e_sync_udp_filters(pf);

	return ret;
}