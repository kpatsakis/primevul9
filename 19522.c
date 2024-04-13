static int iwl_trans_pcie_wait_txqs_empty(struct iwl_trans *trans, u32 txq_bm)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int cnt;
	int ret = 0;

	/* waiting for all the tx frames complete might take a while */
	for (cnt = 0;
	     cnt < trans->trans_cfg->base_params->num_of_queues;
	     cnt++) {

		if (cnt == trans_pcie->cmd_queue)
			continue;
		if (!test_bit(cnt, trans_pcie->queue_used))
			continue;
		if (!(BIT(cnt) & txq_bm))
			continue;

		ret = iwl_trans_pcie_wait_txq_empty(trans, cnt);
		if (ret)
			break;
	}

	return ret;
}