static int i40e_add_veb(struct i40e_veb *veb, struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = veb->pf;
	bool enable_stats = !!(pf->flags & I40E_FLAG_VEB_STATS_ENABLED);
	int ret;

	ret = i40e_aq_add_veb(&pf->hw, veb->uplink_seid, vsi->seid,
			      veb->enabled_tc, false,
			      &veb->seid, enable_stats, NULL);

	/* get a VEB from the hardware */
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "couldn't add VEB, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		return -EPERM;
	}

	/* get statistics counter */
	ret = i40e_aq_get_veb_parameters(&pf->hw, veb->seid, NULL, NULL,
					 &veb->stats_idx, NULL, NULL, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "couldn't get VEB statistics idx, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		return -EPERM;
	}
	ret = i40e_veb_get_bw_info(veb);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "couldn't get VEB bw info, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		i40e_aq_delete_element(&pf->hw, veb->seid, NULL);
		return -ENOENT;
	}

	vsi->uplink_seid = veb->seid;
	vsi->veb_idx = veb->idx;
	vsi->flags |= I40E_VSI_FLAG_VEB_OWNER;

	return 0;
}