static inline int i40e_setup_hw_channel(struct i40e_pf *pf,
					struct i40e_vsi *vsi,
					struct i40e_channel *ch,
					u16 uplink_seid, u8 type)
{
	int ret;

	ch->initialized = false;
	ch->base_queue = vsi->next_base_queue;
	ch->type = type;

	/* Proceed with creation of channel (VMDq2) VSI */
	ret = i40e_add_channel(pf, uplink_seid, ch);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "failed to add_channel using uplink_seid %u\n",
			 uplink_seid);
		return ret;
	}

	/* Mark the successful creation of channel */
	ch->initialized = true;

	/* Reconfigure TX queues using QTX_CTL register */
	ret = i40e_channel_config_tx_ring(pf, vsi, ch);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "failed to configure TX rings for channel %u\n",
			 ch->seid);
		return ret;
	}

	/* update 'next_base_queue' */
	vsi->next_base_queue = vsi->next_base_queue + ch->num_queue_pairs;
	dev_dbg(&pf->pdev->dev,
		"Added channel: vsi_seid %u, vsi_number %u, stat_counter_idx %u, num_queue_pairs %u, pf->next_base_queue %d\n",
		ch->seid, ch->vsi_number, ch->stat_counter_idx,
		ch->num_queue_pairs,
		vsi->next_base_queue);
	return ret;
}