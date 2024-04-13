static void i40e_remove_queue_channels(struct i40e_vsi *vsi)
{
	enum i40e_admin_queue_err last_aq_status;
	struct i40e_cloud_filter *cfilter;
	struct i40e_channel *ch, *ch_tmp;
	struct i40e_pf *pf = vsi->back;
	struct hlist_node *node;
	int ret, i;

	/* Reset rss size that was stored when reconfiguring rss for
	 * channel VSIs with non-power-of-2 queue count.
	 */
	vsi->current_rss_size = 0;

	/* perform cleanup for channels if they exist */
	if (list_empty(&vsi->ch_list))
		return;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->ch_list, list) {
		struct i40e_vsi *p_vsi;

		list_del(&ch->list);
		p_vsi = ch->parent_vsi;
		if (!p_vsi || !ch->initialized) {
			kfree(ch);
			continue;
		}
		/* Reset queue contexts */
		for (i = 0; i < ch->num_queue_pairs; i++) {
			struct i40e_ring *tx_ring, *rx_ring;
			u16 pf_q;

			pf_q = ch->base_queue + i;
			tx_ring = vsi->tx_rings[pf_q];
			tx_ring->ch = NULL;

			rx_ring = vsi->rx_rings[pf_q];
			rx_ring->ch = NULL;
		}

		/* Reset BW configured for this VSI via mqprio */
		ret = i40e_set_bw_limit(vsi, ch->seid, 0);
		if (ret)
			dev_info(&vsi->back->pdev->dev,
				 "Failed to reset tx rate for ch->seid %u\n",
				 ch->seid);

		/* delete cloud filters associated with this channel */
		hlist_for_each_entry_safe(cfilter, node,
					  &pf->cloud_filter_list, cloud_node) {
			if (cfilter->seid != ch->seid)
				continue;

			hash_del(&cfilter->cloud_node);
			if (cfilter->dst_port)
				ret = i40e_add_del_cloud_filter_big_buf(vsi,
									cfilter,
									false);
			else
				ret = i40e_add_del_cloud_filter(vsi, cfilter,
								false);
			last_aq_status = pf->hw.aq.asq_last_status;
			if (ret)
				dev_info(&pf->pdev->dev,
					 "Failed to delete cloud filter, err %s aq_err %s\n",
					 i40e_stat_str(&pf->hw, ret),
					 i40e_aq_str(&pf->hw, last_aq_status));
			kfree(cfilter);
		}

		/* delete VSI from FW */
		ret = i40e_aq_delete_element(&vsi->back->hw, ch->seid,
					     NULL);
		if (ret)
			dev_err(&vsi->back->pdev->dev,
				"unable to remove channel (%d) for parent VSI(%d)\n",
				ch->seid, p_vsi->seid);
		kfree(ch);
	}
	INIT_LIST_HEAD(&vsi->ch_list);
}