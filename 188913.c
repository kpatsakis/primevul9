static void i40e_channel_setup_queue_map(struct i40e_pf *pf,
					 struct i40e_vsi_context *ctxt,
					 struct i40e_channel *ch)
{
	u16 qcount, qmap, sections = 0;
	u8 offset = 0;
	int pow;

	sections = I40E_AQ_VSI_PROP_QUEUE_MAP_VALID;
	sections |= I40E_AQ_VSI_PROP_SCHED_VALID;

	qcount = min_t(int, ch->num_queue_pairs, pf->num_lan_msix);
	ch->num_queue_pairs = qcount;

	/* find the next higher power-of-2 of num queue pairs */
	pow = ilog2(qcount);
	if (!is_power_of_2(qcount))
		pow++;

	qmap = (offset << I40E_AQ_VSI_TC_QUE_OFFSET_SHIFT) |
		(pow << I40E_AQ_VSI_TC_QUE_NUMBER_SHIFT);

	/* Setup queue TC[0].qmap for given VSI context */
	ctxt->info.tc_mapping[0] = cpu_to_le16(qmap);

	ctxt->info.up_enable_bits = 0x1; /* TC0 enabled */
	ctxt->info.mapping_flags |= cpu_to_le16(I40E_AQ_VSI_QUE_MAP_CONTIG);
	ctxt->info.queue_mapping[0] = cpu_to_le16(ch->base_queue);
	ctxt->info.valid_sections |= cpu_to_le16(sections);
}