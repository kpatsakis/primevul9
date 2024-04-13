int i40e_veb_config_tc(struct i40e_veb *veb, u8 enabled_tc)
{
	struct i40e_aqc_configure_switching_comp_bw_config_data bw_data = {0};
	struct i40e_pf *pf = veb->pf;
	int ret = 0;
	int i;

	/* No TCs or already enabled TCs just return */
	if (!enabled_tc || veb->enabled_tc == enabled_tc)
		return ret;

	bw_data.tc_valid_bits = enabled_tc;
	/* bw_data.absolute_credits is not set (relative) */

	/* Enable ETS TCs with equal BW Share for now */
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (enabled_tc & BIT(i))
			bw_data.tc_bw_share_credits[i] = 1;
	}

	ret = i40e_aq_config_switch_comp_bw_config(&pf->hw, veb->seid,
						   &bw_data, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "VEB bw config failed, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		goto out;
	}

	/* Update the BW information */
	ret = i40e_veb_get_bw_info(veb);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Failed getting veb bw config, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
	}

out:
	return ret;
}