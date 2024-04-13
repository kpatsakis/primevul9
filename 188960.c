static int i40e_vsi_configure_bw_alloc(struct i40e_vsi *vsi, u8 enabled_tc,
				       u8 *bw_share)
{
	struct i40e_aqc_configure_vsi_tc_bw_data bw_data;
	struct i40e_pf *pf = vsi->back;
	i40e_status ret;
	int i;

	/* There is no need to reset BW when mqprio mode is on.  */
	if (pf->flags & I40E_FLAG_TC_MQPRIO)
		return 0;
	if (!vsi->mqprio_qopt.qopt.hw && !(pf->flags & I40E_FLAG_DCB_ENABLED)) {
		ret = i40e_set_bw_limit(vsi, vsi->seid, 0);
		if (ret)
			dev_info(&pf->pdev->dev,
				 "Failed to reset tx rate for vsi->seid %u\n",
				 vsi->seid);
		return ret;
	}
	bw_data.tc_valid_bits = enabled_tc;
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++)
		bw_data.tc_bw_credits[i] = bw_share[i];

	ret = i40e_aq_config_vsi_tc_bw(&pf->hw, vsi->seid, &bw_data, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "AQ command Config VSI BW allocation per TC failed = %d\n",
			 pf->hw.aq.asq_last_status);
		return -EINVAL;
	}

	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++)
		vsi->info.qs_handle[i] = bw_data.qs_handles[i];

	return 0;
}