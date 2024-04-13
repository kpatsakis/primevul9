static int iwl_trans_pcie_wait_txq_empty(struct iwl_trans *trans, int txq_idx)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_txq *txq;
	unsigned long now = jiffies;
	bool overflow_tx;
	u8 wr_ptr;

	/* Make sure the NIC is still alive in the bus */
	if (test_bit(STATUS_TRANS_DEAD, &trans->status))
		return -ENODEV;

	if (!test_bit(txq_idx, trans_pcie->queue_used))
		return -EINVAL;

	IWL_DEBUG_TX_QUEUES(trans, "Emptying queue %d...\n", txq_idx);
	txq = trans_pcie->txq[txq_idx];

	spin_lock_bh(&txq->lock);
	overflow_tx = txq->overflow_tx ||
		      !skb_queue_empty(&txq->overflow_q);
	spin_unlock_bh(&txq->lock);

	wr_ptr = READ_ONCE(txq->write_ptr);

	while ((txq->read_ptr != READ_ONCE(txq->write_ptr) ||
		overflow_tx) &&
	       !time_after(jiffies,
			   now + msecs_to_jiffies(IWL_FLUSH_WAIT_MS))) {
		u8 write_ptr = READ_ONCE(txq->write_ptr);

		/*
		 * If write pointer moved during the wait, warn only
		 * if the TX came from op mode. In case TX came from
		 * trans layer (overflow TX) don't warn.
		 */
		if (WARN_ONCE(wr_ptr != write_ptr && !overflow_tx,
			      "WR pointer moved while flushing %d -> %d\n",
			      wr_ptr, write_ptr))
			return -ETIMEDOUT;
		wr_ptr = write_ptr;

		usleep_range(1000, 2000);

		spin_lock_bh(&txq->lock);
		overflow_tx = txq->overflow_tx ||
			      !skb_queue_empty(&txq->overflow_q);
		spin_unlock_bh(&txq->lock);
	}

	if (txq->read_ptr != txq->write_ptr) {
		IWL_ERR(trans,
			"fail to flush all tx fifo queues Q %d\n", txq_idx);
		iwl_trans_pcie_log_scd_error(trans, txq);
		return -ETIMEDOUT;
	}

	IWL_DEBUG_TX_QUEUES(trans, "Queue %d is now empty.\n", txq_idx);

	return 0;
}