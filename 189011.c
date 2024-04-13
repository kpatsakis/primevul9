static int i40e_vsi_setup_queue_map_mqprio(struct i40e_vsi *vsi,
					   struct i40e_vsi_context *ctxt,
					   u8 enabled_tc)
{
	u16 qcount = 0, max_qcount, qmap, sections = 0;
	int i, override_q, pow, num_qps, ret;
	u8 netdev_tc = 0, offset = 0;

	if (vsi->type != I40E_VSI_MAIN)
		return -EINVAL;
	sections = I40E_AQ_VSI_PROP_QUEUE_MAP_VALID;
	sections |= I40E_AQ_VSI_PROP_SCHED_VALID;
	vsi->tc_config.numtc = vsi->mqprio_qopt.qopt.num_tc;
	vsi->tc_config.enabled_tc = enabled_tc ? enabled_tc : 1;
	num_qps = vsi->mqprio_qopt.qopt.count[0];

	/* find the next higher power-of-2 of num queue pairs */
	pow = ilog2(num_qps);
	if (!is_power_of_2(num_qps))
		pow++;
	qmap = (offset << I40E_AQ_VSI_TC_QUE_OFFSET_SHIFT) |
		(pow << I40E_AQ_VSI_TC_QUE_NUMBER_SHIFT);

	/* Setup queue offset/count for all TCs for given VSI */
	max_qcount = vsi->mqprio_qopt.qopt.count[0];
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		/* See if the given TC is enabled for the given VSI */
		if (vsi->tc_config.enabled_tc & BIT(i)) {
			offset = vsi->mqprio_qopt.qopt.offset[i];
			qcount = vsi->mqprio_qopt.qopt.count[i];
			if (qcount > max_qcount)
				max_qcount = qcount;
			vsi->tc_config.tc_info[i].qoffset = offset;
			vsi->tc_config.tc_info[i].qcount = qcount;
			vsi->tc_config.tc_info[i].netdev_tc = netdev_tc++;
		} else {
			/* TC is not enabled so set the offset to
			 * default queue and allocate one queue
			 * for the given TC.
			 */
			vsi->tc_config.tc_info[i].qoffset = 0;
			vsi->tc_config.tc_info[i].qcount = 1;
			vsi->tc_config.tc_info[i].netdev_tc = 0;
		}
	}

	/* Set actual Tx/Rx queue pairs */
	vsi->num_queue_pairs = offset + qcount;

	/* Setup queue TC[0].qmap for given VSI context */
	ctxt->info.tc_mapping[0] = cpu_to_le16(qmap);
	ctxt->info.mapping_flags |= cpu_to_le16(I40E_AQ_VSI_QUE_MAP_CONTIG);
	ctxt->info.queue_mapping[0] = cpu_to_le16(vsi->base_queue);
	ctxt->info.valid_sections |= cpu_to_le16(sections);

	/* Reconfigure RSS for main VSI with max queue count */
	vsi->rss_size = max_qcount;
	ret = i40e_vsi_config_rss(vsi);
	if (ret) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed to reconfig rss for num_queues (%u)\n",
			 max_qcount);
		return ret;
	}
	vsi->reconfig_rss = true;
	dev_dbg(&vsi->back->pdev->dev,
		"Reconfigured rss with num_queues (%u)\n", max_qcount);

	/* Find queue count available for channel VSIs and starting offset
	 * for channel VSIs
	 */
	override_q = vsi->mqprio_qopt.qopt.count[0];
	if (override_q && override_q < vsi->num_queue_pairs) {
		vsi->cnt_q_avail = vsi->num_queue_pairs - override_q;
		vsi->next_base_queue = override_q;
	}
	return 0;
}