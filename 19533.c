static void iwl_trans_pcie_block_txq_ptrs(struct iwl_trans *trans, bool block)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int i;

	for (i = 0; i < trans->trans_cfg->base_params->num_of_queues; i++) {
		struct iwl_txq *txq = trans_pcie->txq[i];

		if (i == trans_pcie->cmd_queue)
			continue;

		spin_lock_bh(&txq->lock);

		if (!block && !(WARN_ON_ONCE(!txq->block))) {
			txq->block--;
			if (!txq->block) {
				iwl_write32(trans, HBUS_TARG_WRPTR,
					    txq->write_ptr | (i << 8));
			}
		} else if (block) {
			txq->block++;
		}

		spin_unlock_bh(&txq->lock);
	}
}