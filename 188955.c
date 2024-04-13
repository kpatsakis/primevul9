static void i40e_determine_queue_usage(struct i40e_pf *pf)
{
	int queues_left;
	int q_max;

	pf->num_lan_qps = 0;

	/* Find the max queues to be put into basic use.  We'll always be
	 * using TC0, whether or not DCB is running, and TC0 will get the
	 * big RSS set.
	 */
	queues_left = pf->hw.func_caps.num_tx_qp;

	if ((queues_left == 1) ||
	    !(pf->flags & I40E_FLAG_MSIX_ENABLED)) {
		/* one qp for PF, no queues for anything else */
		queues_left = 0;
		pf->alloc_rss_size = pf->num_lan_qps = 1;

		/* make sure all the fancies are disabled */
		pf->flags &= ~(I40E_FLAG_RSS_ENABLED	|
			       I40E_FLAG_IWARP_ENABLED	|
			       I40E_FLAG_FD_SB_ENABLED	|
			       I40E_FLAG_FD_ATR_ENABLED	|
			       I40E_FLAG_DCB_CAPABLE	|
			       I40E_FLAG_DCB_ENABLED	|
			       I40E_FLAG_SRIOV_ENABLED	|
			       I40E_FLAG_VMDQ_ENABLED);
		pf->flags |= I40E_FLAG_FD_SB_INACTIVE;
	} else if (!(pf->flags & (I40E_FLAG_RSS_ENABLED |
				  I40E_FLAG_FD_SB_ENABLED |
				  I40E_FLAG_FD_ATR_ENABLED |
				  I40E_FLAG_DCB_CAPABLE))) {
		/* one qp for PF */
		pf->alloc_rss_size = pf->num_lan_qps = 1;
		queues_left -= pf->num_lan_qps;

		pf->flags &= ~(I40E_FLAG_RSS_ENABLED	|
			       I40E_FLAG_IWARP_ENABLED	|
			       I40E_FLAG_FD_SB_ENABLED	|
			       I40E_FLAG_FD_ATR_ENABLED	|
			       I40E_FLAG_DCB_ENABLED	|
			       I40E_FLAG_VMDQ_ENABLED);
		pf->flags |= I40E_FLAG_FD_SB_INACTIVE;
	} else {
		/* Not enough queues for all TCs */
		if ((pf->flags & I40E_FLAG_DCB_CAPABLE) &&
		    (queues_left < I40E_MAX_TRAFFIC_CLASS)) {
			pf->flags &= ~(I40E_FLAG_DCB_CAPABLE |
					I40E_FLAG_DCB_ENABLED);
			dev_info(&pf->pdev->dev, "not enough queues for DCB. DCB is disabled.\n");
		}

		/* limit lan qps to the smaller of qps, cpus or msix */
		q_max = max_t(int, pf->rss_size_max, num_online_cpus());
		q_max = min_t(int, q_max, pf->hw.func_caps.num_tx_qp);
		q_max = min_t(int, q_max, pf->hw.func_caps.num_msix_vectors);
		pf->num_lan_qps = q_max;

		queues_left -= pf->num_lan_qps;
	}

	if (pf->flags & I40E_FLAG_FD_SB_ENABLED) {
		if (queues_left > 1) {
			queues_left -= 1; /* save 1 queue for FD */
		} else {
			pf->flags &= ~I40E_FLAG_FD_SB_ENABLED;
			pf->flags |= I40E_FLAG_FD_SB_INACTIVE;
			dev_info(&pf->pdev->dev, "not enough queues for Flow Director. Flow Director feature is disabled\n");
		}
	}

	if ((pf->flags & I40E_FLAG_SRIOV_ENABLED) &&
	    pf->num_vf_qps && pf->num_req_vfs && queues_left) {
		pf->num_req_vfs = min_t(int, pf->num_req_vfs,
					(queues_left / pf->num_vf_qps));
		queues_left -= (pf->num_req_vfs * pf->num_vf_qps);
	}

	if ((pf->flags & I40E_FLAG_VMDQ_ENABLED) &&
	    pf->num_vmdq_vsis && pf->num_vmdq_qps && queues_left) {
		pf->num_vmdq_vsis = min_t(int, pf->num_vmdq_vsis,
					  (queues_left / pf->num_vmdq_qps));
		queues_left -= (pf->num_vmdq_vsis * pf->num_vmdq_qps);
	}

	pf->queues_left = queues_left;
	dev_dbg(&pf->pdev->dev,
		"qs_avail=%d FD SB=%d lan_qs=%d lan_tc0=%d vf=%d*%d vmdq=%d*%d, remaining=%d\n",
		pf->hw.func_caps.num_tx_qp,
		!!(pf->flags & I40E_FLAG_FD_SB_ENABLED),
		pf->num_lan_qps, pf->alloc_rss_size, pf->num_req_vfs,
		pf->num_vf_qps, pf->num_vmdq_vsis, pf->num_vmdq_qps,
		queues_left);
}