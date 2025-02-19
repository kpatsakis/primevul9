static int i40e_setup_macvlans(struct i40e_vsi *vsi, u16 macvlan_cnt, u16 qcnt,
			       struct net_device *vdev)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	struct i40e_vsi_context ctxt;
	u16 sections, qmap, num_qps;
	struct i40e_channel *ch;
	int i, pow, ret = 0;
	u8 offset = 0;

	if (vsi->type != I40E_VSI_MAIN || !macvlan_cnt)
		return -EINVAL;

	num_qps = vsi->num_queue_pairs - (macvlan_cnt * qcnt);

	/* find the next higher power-of-2 of num queue pairs */
	pow = fls(roundup_pow_of_two(num_qps) - 1);

	qmap = (offset << I40E_AQ_VSI_TC_QUE_OFFSET_SHIFT) |
		(pow << I40E_AQ_VSI_TC_QUE_NUMBER_SHIFT);

	/* Setup context bits for the main VSI */
	sections = I40E_AQ_VSI_PROP_QUEUE_MAP_VALID;
	sections |= I40E_AQ_VSI_PROP_SCHED_VALID;
	memset(&ctxt, 0, sizeof(ctxt));
	ctxt.seid = vsi->seid;
	ctxt.pf_num = vsi->back->hw.pf_id;
	ctxt.vf_num = 0;
	ctxt.uplink_seid = vsi->uplink_seid;
	ctxt.info = vsi->info;
	ctxt.info.tc_mapping[0] = cpu_to_le16(qmap);
	ctxt.info.mapping_flags |= cpu_to_le16(I40E_AQ_VSI_QUE_MAP_CONTIG);
	ctxt.info.queue_mapping[0] = cpu_to_le16(vsi->base_queue);
	ctxt.info.valid_sections |= cpu_to_le16(sections);

	/* Reconfigure RSS for main VSI with new max queue count */
	vsi->rss_size = max_t(u16, num_qps, qcnt);
	ret = i40e_vsi_config_rss(vsi);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Failed to reconfig RSS for num_queues (%u)\n",
			 vsi->rss_size);
		return ret;
	}
	vsi->reconfig_rss = true;
	dev_dbg(&vsi->back->pdev->dev,
		"Reconfigured RSS with num_queues (%u)\n", vsi->rss_size);
	vsi->next_base_queue = num_qps;
	vsi->cnt_q_avail = vsi->num_queue_pairs - num_qps;

	/* Update the VSI after updating the VSI queue-mapping
	 * information
	 */
	ret = i40e_aq_update_vsi_params(hw, &ctxt, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Update vsi tc config failed, err %s aq_err %s\n",
			 i40e_stat_str(hw, ret),
			 i40e_aq_str(hw, hw->aq.asq_last_status));
		return ret;
	}
	/* update the local VSI info with updated queue map */
	i40e_vsi_update_queue_map(vsi, &ctxt);
	vsi->info.valid_sections = 0;

	/* Create channels for macvlans */
	INIT_LIST_HEAD(&vsi->macvlan_list);
	for (i = 0; i < macvlan_cnt; i++) {
		ch = kzalloc(sizeof(*ch), GFP_KERNEL);
		if (!ch) {
			ret = -ENOMEM;
			goto err_free;
		}
		INIT_LIST_HEAD(&ch->list);
		ch->num_queue_pairs = qcnt;
		if (!i40e_setup_channel(pf, vsi, ch)) {
			ret = -EINVAL;
			kfree(ch);
			goto err_free;
		}
		ch->parent_vsi = vsi;
		vsi->cnt_q_avail -= ch->num_queue_pairs;
		vsi->macvlan_cnt++;
		list_add_tail(&ch->list, &vsi->macvlan_list);
	}

	return ret;

err_free:
	dev_info(&pf->pdev->dev, "Failed to setup macvlans\n");
	i40e_free_macvlan_channels(vsi);

	return ret;
}