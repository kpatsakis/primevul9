static int i40e_rebuild_cloud_filters(struct i40e_vsi *vsi, u16 seid)
{
	struct i40e_cloud_filter *cfilter;
	struct i40e_pf *pf = vsi->back;
	struct hlist_node *node;
	i40e_status ret;

	/* Add cloud filters back if they exist */
	hlist_for_each_entry_safe(cfilter, node, &pf->cloud_filter_list,
				  cloud_node) {
		if (cfilter->seid != seid)
			continue;

		if (cfilter->dst_port)
			ret = i40e_add_del_cloud_filter_big_buf(vsi, cfilter,
								true);
		else
			ret = i40e_add_del_cloud_filter(vsi, cfilter, true);

		if (ret) {
			dev_dbg(&pf->pdev->dev,
				"Failed to rebuild cloud filter, err %s aq_err %s\n",
				i40e_stat_str(&pf->hw, ret),
				i40e_aq_str(&pf->hw,
					    pf->hw.aq.asq_last_status));
			return ret;
		}
	}
	return 0;
}