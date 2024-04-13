static int i40e_channel_config_bw(struct i40e_vsi *vsi, struct i40e_channel *ch,
				  u8 *bw_share)
{
	struct i40e_aqc_configure_vsi_tc_bw_data bw_data;
	i40e_status ret;
	int i;

	bw_data.tc_valid_bits = ch->enabled_tc;
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++)
		bw_data.tc_bw_credits[i] = bw_share[i];

	ret = i40e_aq_config_vsi_tc_bw(&vsi->back->hw, ch->seid,
				       &bw_data, NULL);
	if (ret) {
		dev_info(&vsi->back->pdev->dev,
			 "Config VSI BW allocation per TC failed, aq_err: %d for new_vsi->seid %u\n",
			 vsi->back->hw.aq.asq_last_status, ch->seid);
		return -EINVAL;
	}

	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++)
		ch->info.qs_handle[i] = bw_data.qs_handles[i];

	return 0;
}