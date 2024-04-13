static int i40e_vsi_config_tc(struct i40e_vsi *vsi, u8 enabled_tc)
{
	u8 bw_share[I40E_MAX_TRAFFIC_CLASS] = {0};
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	struct i40e_vsi_context ctxt;
	int ret = 0;
	int i;

	/* Check if enabled_tc is same as existing or new TCs */
	if (vsi->tc_config.enabled_tc == enabled_tc &&
	    vsi->mqprio_qopt.mode != TC_MQPRIO_MODE_CHANNEL)
		return ret;

	/* Enable ETS TCs with equal BW Share for now across all VSIs */
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (enabled_tc & BIT(i))
			bw_share[i] = 1;
	}

	ret = i40e_vsi_configure_bw_alloc(vsi, enabled_tc, bw_share);
	if (ret) {
		struct i40e_aqc_query_vsi_bw_config_resp bw_config = {0};

		dev_info(&pf->pdev->dev,
			 "Failed configuring TC map %d for VSI %d\n",
			 enabled_tc, vsi->seid);
		ret = i40e_aq_query_vsi_bw_config(hw, vsi->seid,
						  &bw_config, NULL);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "Failed querying vsi bw info, err %s aq_err %s\n",
				 i40e_stat_str(hw, ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
			goto out;
		}
		if ((bw_config.tc_valid_bits & enabled_tc) != enabled_tc) {
			u8 valid_tc = bw_config.tc_valid_bits & enabled_tc;

			if (!valid_tc)
				valid_tc = bw_config.tc_valid_bits;
			/* Always enable TC0, no matter what */
			valid_tc |= 1;
			dev_info(&pf->pdev->dev,
				 "Requested tc 0x%x, but FW reports 0x%x as valid. Attempting to use 0x%x.\n",
				 enabled_tc, bw_config.tc_valid_bits, valid_tc);
			enabled_tc = valid_tc;
		}

		ret = i40e_vsi_configure_bw_alloc(vsi, enabled_tc, bw_share);
		if (ret) {
			dev_err(&pf->pdev->dev,
				"Unable to  configure TC map %d for VSI %d\n",
				enabled_tc, vsi->seid);
			goto out;
		}
	}

	/* Update Queue Pairs Mapping for currently enabled UPs */
	ctxt.seid = vsi->seid;
	ctxt.pf_num = vsi->back->hw.pf_id;
	ctxt.vf_num = 0;
	ctxt.uplink_seid = vsi->uplink_seid;
	ctxt.info = vsi->info;
	if (vsi->back->flags & I40E_FLAG_TC_MQPRIO) {
		ret = i40e_vsi_setup_queue_map_mqprio(vsi, &ctxt, enabled_tc);
		if (ret)
			goto out;
	} else {
		i40e_vsi_setup_queue_map(vsi, &ctxt, enabled_tc, false);
	}

	/* On destroying the qdisc, reset vsi->rss_size, as number of enabled
	 * queues changed.
	 */
	if (!vsi->mqprio_qopt.qopt.hw && vsi->reconfig_rss) {
		vsi->rss_size = min_t(int, vsi->back->alloc_rss_size,
				      vsi->num_queue_pairs);
		ret = i40e_vsi_config_rss(vsi);
		if (ret) {
			dev_info(&vsi->back->pdev->dev,
				 "Failed to reconfig rss for num_queues\n");
			return ret;
		}
		vsi->reconfig_rss = false;
	}
	if (vsi->back->flags & I40E_FLAG_IWARP_ENABLED) {
		ctxt.info.valid_sections |=
				cpu_to_le16(I40E_AQ_VSI_PROP_QUEUE_OPT_VALID);
		ctxt.info.queueing_opt_flags |= I40E_AQ_VSI_QUE_OPT_TCP_ENA;
	}

	/* Update the VSI after updating the VSI queue-mapping
	 * information
	 */
	ret = i40e_aq_update_vsi_params(hw, &ctxt, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Update vsi tc config failed, err %s aq_err %s\n",
			 i40e_stat_str(hw, ret),
			 i40e_aq_str(hw, hw->aq.asq_last_status));
		goto out;
	}
	/* update the local VSI info with updated queue map */
	i40e_vsi_update_queue_map(vsi, &ctxt);
	vsi->info.valid_sections = 0;

	/* Update current VSI BW information */
	ret = i40e_vsi_get_bw_info(vsi);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Failed updating vsi bw info, err %s aq_err %s\n",
			 i40e_stat_str(hw, ret),
			 i40e_aq_str(hw, hw->aq.asq_last_status));
		goto out;
	}

	/* Update the netdev TC setup */
	i40e_vsi_config_netdev_tc(vsi, enabled_tc);
out:
	return ret;
}