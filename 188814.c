static int i40e_rebuild_channels(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch, *ch_tmp;
	i40e_status ret;

	if (list_empty(&vsi->ch_list))
		return 0;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->ch_list, list) {
		if (!ch->initialized)
			break;
		/* Proceed with creation of channel (VMDq2) VSI */
		ret = i40e_add_channel(vsi->back, vsi->uplink_seid, ch);
		if (ret) {
			dev_info(&vsi->back->pdev->dev,
				 "failed to rebuild channels using uplink_seid %u\n",
				 vsi->uplink_seid);
			return ret;
		}
		/* Reconfigure TX queues using QTX_CTL register */
		ret = i40e_channel_config_tx_ring(vsi->back, vsi, ch);
		if (ret) {
			dev_info(&vsi->back->pdev->dev,
				 "failed to configure TX rings for channel %u\n",
				 ch->seid);
			return ret;
		}
		/* update 'next_base_queue' */
		vsi->next_base_queue = vsi->next_base_queue +
							ch->num_queue_pairs;
		if (ch->max_tx_rate) {
			u64 credits = ch->max_tx_rate;

			if (i40e_set_bw_limit(vsi, ch->seid,
					      ch->max_tx_rate))
				return -EINVAL;

			do_div(credits, I40E_BW_CREDIT_DIVISOR);
			dev_dbg(&vsi->back->pdev->dev,
				"Set tx rate of %llu Mbps (count of 50Mbps %llu) for vsi->seid %u\n",
				ch->max_tx_rate,
				credits,
				ch->seid);
		}
		ret = i40e_rebuild_cloud_filters(vsi, ch->seid);
		if (ret) {
			dev_dbg(&vsi->back->pdev->dev,
				"Failed to rebuild cloud filters for channel VSI %u\n",
				ch->seid);
			return ret;
		}
	}
	return 0;
}