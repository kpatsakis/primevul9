int i40e_set_bw_limit(struct i40e_vsi *vsi, u16 seid, u64 max_tx_rate)
{
	struct i40e_pf *pf = vsi->back;
	u64 credits = 0;
	int speed = 0;
	int ret = 0;

	speed = i40e_get_link_speed(vsi);
	if (max_tx_rate > speed) {
		dev_err(&pf->pdev->dev,
			"Invalid max tx rate %llu specified for VSI seid %d.",
			max_tx_rate, seid);
		return -EINVAL;
	}
	if (max_tx_rate && max_tx_rate < 50) {
		dev_warn(&pf->pdev->dev,
			 "Setting max tx rate to minimum usable value of 50Mbps.\n");
		max_tx_rate = 50;
	}

	/* Tx rate credits are in values of 50Mbps, 0 is disabled */
	credits = max_tx_rate;
	do_div(credits, I40E_BW_CREDIT_DIVISOR);
	ret = i40e_aq_config_vsi_bw_limit(&pf->hw, seid, credits,
					  I40E_MAX_BW_INACTIVE_ACCUM, NULL);
	if (ret)
		dev_err(&pf->pdev->dev,
			"Failed set tx rate (%llu Mbps) for vsi->seid %u, err %s aq_err %s\n",
			max_tx_rate, seid, i40e_stat_str(&pf->hw, ret),
			i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
	return ret;
}