int i40e_create_queue_channel(struct i40e_vsi *vsi,
			      struct i40e_channel *ch)
{
	struct i40e_pf *pf = vsi->back;
	bool reconfig_rss;
	int err;

	if (!ch)
		return -EINVAL;

	if (!ch->num_queue_pairs) {
		dev_err(&pf->pdev->dev, "Invalid num_queues requested: %d\n",
			ch->num_queue_pairs);
		return -EINVAL;
	}

	/* validate user requested num_queues for channel */
	err = i40e_validate_num_queues(pf, ch->num_queue_pairs, vsi,
				       &reconfig_rss);
	if (err) {
		dev_info(&pf->pdev->dev, "Failed to validate num_queues (%d)\n",
			 ch->num_queue_pairs);
		return -EINVAL;
	}

	/* By default we are in VEPA mode, if this is the first VF/VMDq
	 * VSI to be added switch to VEB mode.
	 */
	if ((!(pf->flags & I40E_FLAG_VEB_MODE_ENABLED)) ||
	    (!i40e_is_any_channel(vsi))) {
		if (!is_power_of_2(vsi->tc_config.tc_info[0].qcount)) {
			dev_dbg(&pf->pdev->dev,
				"Failed to create channel. Override queues (%u) not power of 2\n",
				vsi->tc_config.tc_info[0].qcount);
			return -EINVAL;
		}

		if (!(pf->flags & I40E_FLAG_VEB_MODE_ENABLED)) {
			pf->flags |= I40E_FLAG_VEB_MODE_ENABLED;

			if (vsi->type == I40E_VSI_MAIN) {
				if (pf->flags & I40E_FLAG_TC_MQPRIO)
					i40e_do_reset(pf, I40E_PF_RESET_FLAG,
						      true);
				else
					i40e_do_reset_safe(pf,
							   I40E_PF_RESET_FLAG);
			}
		}
		/* now onwards for main VSI, number of queues will be value
		 * of TC0's queue count
		 */
	}

	/* By this time, vsi->cnt_q_avail shall be set to non-zero and
	 * it should be more than num_queues
	 */
	if (!vsi->cnt_q_avail || vsi->cnt_q_avail < ch->num_queue_pairs) {
		dev_dbg(&pf->pdev->dev,
			"Error: cnt_q_avail (%u) less than num_queues %d\n",
			vsi->cnt_q_avail, ch->num_queue_pairs);
		return -EINVAL;
	}

	/* reconfig_rss only if vsi type is MAIN_VSI */
	if (reconfig_rss && (vsi->type == I40E_VSI_MAIN)) {
		err = i40e_vsi_reconfig_rss(vsi, ch->num_queue_pairs);
		if (err) {
			dev_info(&pf->pdev->dev,
				 "Error: unable to reconfig rss for num_queues (%u)\n",
				 ch->num_queue_pairs);
			return -EINVAL;
		}
	}

	if (!i40e_setup_channel(pf, vsi, ch)) {
		dev_info(&pf->pdev->dev, "Failed to setup channel\n");
		return -EINVAL;
	}

	dev_info(&pf->pdev->dev,
		 "Setup channel (id:%u) utilizing num_queues %d\n",
		 ch->seid, ch->num_queue_pairs);

	/* configure VSI for BW limit */
	if (ch->max_tx_rate) {
		u64 credits = ch->max_tx_rate;

		if (i40e_set_bw_limit(vsi, ch->seid, ch->max_tx_rate))
			return -EINVAL;

		do_div(credits, I40E_BW_CREDIT_DIVISOR);
		dev_dbg(&pf->pdev->dev,
			"Set tx rate of %llu Mbps (count of 50Mbps %llu) for vsi->seid %u\n",
			ch->max_tx_rate,
			credits,
			ch->seid);
	}

	/* in case of VF, this will be main SRIOV VSI */
	ch->parent_vsi = vsi;

	/* and update main_vsi's count for queue_available to use */
	vsi->cnt_q_avail -= ch->num_queue_pairs;

	return 0;
}