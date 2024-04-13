int i40e_vsi_wait_queues_disabled(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	int i, pf_q, ret;

	pf_q = vsi->base_queue;
	for (i = 0; i < vsi->num_queue_pairs; i++, pf_q++) {
		/* Check and wait for the Tx queue */
		ret = i40e_pf_txq_wait(pf, pf_q, false);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "VSI seid %d Tx ring %d disable timeout\n",
				 vsi->seid, pf_q);
			return ret;
		}

		if (!i40e_enabled_xdp_vsi(vsi))
			goto wait_rx;

		/* Check and wait for the XDP Tx queue */
		ret = i40e_pf_txq_wait(pf, pf_q + vsi->alloc_queue_pairs,
				       false);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "VSI seid %d XDP Tx ring %d disable timeout\n",
				 vsi->seid, pf_q);
			return ret;
		}
wait_rx:
		/* Check and wait for the Rx queue */
		ret = i40e_pf_rxq_wait(pf, pf_q, false);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "VSI seid %d Rx ring %d disable timeout\n",
				 vsi->seid, pf_q);
			return ret;
		}
	}

	return 0;
}