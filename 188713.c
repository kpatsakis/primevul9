static int i40e_handle_lldp_event(struct i40e_pf *pf,
				  struct i40e_arq_event_info *e)
{
	struct i40e_aqc_lldp_get_mib *mib =
		(struct i40e_aqc_lldp_get_mib *)&e->desc.params.raw;
	struct i40e_hw *hw = &pf->hw;
	struct i40e_dcbx_config tmp_dcbx_cfg;
	bool need_reconfig = false;
	int ret = 0;
	u8 type;

	/* Not DCB capable or capability disabled */
	if (!(pf->flags & I40E_FLAG_DCB_CAPABLE))
		return ret;

	/* Ignore if event is not for Nearest Bridge */
	type = ((mib->type >> I40E_AQ_LLDP_BRIDGE_TYPE_SHIFT)
		& I40E_AQ_LLDP_BRIDGE_TYPE_MASK);
	dev_dbg(&pf->pdev->dev, "LLDP event mib bridge type 0x%x\n", type);
	if (type != I40E_AQ_LLDP_BRIDGE_TYPE_NEAREST_BRIDGE)
		return ret;

	/* Check MIB Type and return if event for Remote MIB update */
	type = mib->type & I40E_AQ_LLDP_MIB_TYPE_MASK;
	dev_dbg(&pf->pdev->dev,
		"LLDP event mib type %s\n", type ? "remote" : "local");
	if (type == I40E_AQ_LLDP_MIB_REMOTE) {
		/* Update the remote cached instance and return */
		ret = i40e_aq_get_dcb_config(hw, I40E_AQ_LLDP_MIB_REMOTE,
				I40E_AQ_LLDP_BRIDGE_TYPE_NEAREST_BRIDGE,
				&hw->remote_dcbx_config);
		goto exit;
	}

	/* Store the old configuration */
	tmp_dcbx_cfg = hw->local_dcbx_config;

	/* Reset the old DCBx configuration data */
	memset(&hw->local_dcbx_config, 0, sizeof(hw->local_dcbx_config));
	/* Get updated DCBX data from firmware */
	ret = i40e_get_dcb_config(&pf->hw);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Failed querying DCB configuration data from firmware, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		goto exit;
	}

	/* No change detected in DCBX configs */
	if (!memcmp(&tmp_dcbx_cfg, &hw->local_dcbx_config,
		    sizeof(tmp_dcbx_cfg))) {
		dev_dbg(&pf->pdev->dev, "No change detected in DCBX configuration.\n");
		goto exit;
	}

	need_reconfig = i40e_dcb_need_reconfig(pf, &tmp_dcbx_cfg,
					       &hw->local_dcbx_config);

	i40e_dcbnl_flush_apps(pf, &tmp_dcbx_cfg, &hw->local_dcbx_config);

	if (!need_reconfig)
		goto exit;

	/* Enable DCB tagging only when more than one TC */
	if (i40e_dcb_get_num_tc(&hw->local_dcbx_config) > 1)
		pf->flags |= I40E_FLAG_DCB_ENABLED;
	else
		pf->flags &= ~I40E_FLAG_DCB_ENABLED;

	set_bit(__I40E_PORT_SUSPENDED, pf->state);
	/* Reconfiguration needed quiesce all VSIs */
	i40e_pf_quiesce_all_vsi(pf);

	/* Changes in configuration update VEB/VSI */
	i40e_dcb_reconfigure(pf);

	ret = i40e_resume_port_tx(pf);

	clear_bit(__I40E_PORT_SUSPENDED, pf->state);
	/* In case of error no point in resuming VSIs */
	if (ret)
		goto exit;

	/* Wait for the PF's queues to be disabled */
	ret = i40e_pf_wait_queues_disabled(pf);
	if (ret) {
		/* Schedule PF reset to recover */
		set_bit(__I40E_PF_RESET_REQUESTED, pf->state);
		i40e_service_event_schedule(pf);
	} else {
		i40e_pf_unquiesce_all_vsi(pf);
		set_bit(__I40E_CLIENT_SERVICE_REQUESTED, pf->state);
		set_bit(__I40E_CLIENT_L2_CHANGE, pf->state);
	}

exit:
	return ret;
}