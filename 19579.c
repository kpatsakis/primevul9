static void iwl_trans_pcie_freeze_txq_timer(struct iwl_trans *trans,
					    unsigned long txqs,
					    bool freeze)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int queue;

	for_each_set_bit(queue, &txqs, BITS_PER_LONG) {
		struct iwl_txq *txq = trans_pcie->txq[queue];
		unsigned long now;

		spin_lock_bh(&txq->lock);

		now = jiffies;

		if (txq->frozen == freeze)
			goto next_queue;

		IWL_DEBUG_TX_QUEUES(trans, "%s TXQ %d\n",
				    freeze ? "Freezing" : "Waking", queue);

		txq->frozen = freeze;

		if (txq->read_ptr == txq->write_ptr)
			goto next_queue;

		if (freeze) {
			if (unlikely(time_after(now,
						txq->stuck_timer.expires))) {
				/*
				 * The timer should have fired, maybe it is
				 * spinning right now on the lock.
				 */
				goto next_queue;
			}
			/* remember how long until the timer fires */
			txq->frozen_expiry_remainder =
				txq->stuck_timer.expires - now;
			del_timer(&txq->stuck_timer);
			goto next_queue;
		}

		/*
		 * Wake a non-empty queue -> arm timer with the
		 * remainder before it froze
		 */
		mod_timer(&txq->stuck_timer,
			  now + txq->frozen_expiry_remainder);

next_queue:
		spin_unlock_bh(&txq->lock);
	}
}