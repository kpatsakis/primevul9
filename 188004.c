static void prb_retire_rx_blk_timer_expired(unsigned long data)
{
	struct packet_sock *po = (struct packet_sock *)data;
	struct kbdq_core *pkc = &po->rx_ring.prb_bdqc;
	unsigned int frozen;
	struct block_desc *pbd;

	spin_lock(&po->sk.sk_receive_queue.lock);

	frozen = prb_queue_frozen(pkc);
	pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	if (unlikely(pkc->delete_blk_timer))
		goto out;

	/* We only need to plug the race when the block is partially filled.
	 * tpacket_rcv:
	 *		lock(); increment BLOCK_NUM_PKTS; unlock()
	 *		copy_bits() is in progress ...
	 *		timer fires on other cpu:
	 *		we can't retire the current block because copy_bits
	 *		is in progress.
	 *
	 */
	if (BLOCK_NUM_PKTS(pbd)) {
		while (atomic_read(&pkc->blk_fill_in_prog)) {
			/* Waiting for skb_copy_bits to finish... */
			cpu_relax();
		}
	}

	if (pkc->last_kactive_blk_num == pkc->kactive_blk_num) {
		if (!frozen) {
			prb_retire_current_block(pkc, po, TP_STATUS_BLK_TMO);
			if (!prb_dispatch_next_block(pkc, po))
				goto refresh_timer;
			else
				goto out;
		} else {
			/* Case 1. Queue was frozen because user-space was
			 *	   lagging behind.
			 */
			if (prb_curr_blk_in_use(pkc, pbd)) {
				/*
				 * Ok, user-space is still behind.
				 * So just refresh the timer.
				 */
				goto refresh_timer;
			} else {
			       /* Case 2. queue was frozen,user-space caught up,
				* now the link went idle && the timer fired.
				* We don't have a block to close.So we open this
				* block and restart the timer.
				* opening a block thaws the queue,restarts timer
				* Thawing/timer-refresh is a side effect.
				*/
				prb_open_block(pkc, pbd);
				goto out;
			}
		}
	}

refresh_timer:
	_prb_refresh_rx_retire_blk_timer(pkc);

out:
	spin_unlock(&po->sk.sk_receive_queue.lock);
}