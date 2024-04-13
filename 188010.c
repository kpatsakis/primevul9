static void *__packet_lookup_frame_in_block(struct packet_sock *po,
					    struct sk_buff *skb,
						int status,
					    unsigned int len
					    )
{
	struct kbdq_core *pkc;
	struct block_desc *pbd;
	char *curr, *end;

	pkc = GET_PBDQC_FROM_RB(((struct packet_ring_buffer *)&po->rx_ring));
	pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	/* Queue is frozen when user space is lagging behind */
	if (prb_queue_frozen(pkc)) {
		/*
		 * Check if that last block which caused the queue to freeze,
		 * is still in_use by user-space.
		 */
		if (prb_curr_blk_in_use(pkc, pbd)) {
			/* Can't record this packet */
			return NULL;
		} else {
			/*
			 * Ok, the block was released by user-space.
			 * Now let's open that block.
			 * opening a block also thaws the queue.
			 * Thawing is a side effect.
			 */
			prb_open_block(pkc, pbd);
		}
	}

	smp_mb();
	curr = pkc->nxt_offset;
	pkc->skb = skb;
	end = (char *) ((char *)pbd + pkc->kblk_size);

	/* first try the current block */
	if (curr+TOTAL_PKT_LEN_INCL_ALIGN(len) < end) {
		prb_fill_curr_block(curr, pkc, pbd, len);
		return (void *)curr;
	}

	/* Ok, close the current block */
	prb_retire_current_block(pkc, po, 0);

	/* Now, try to dispatch the next block */
	curr = (char *)prb_dispatch_next_block(pkc, po);
	if (curr) {
		pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);
		prb_fill_curr_block(curr, pkc, pbd, len);
		return (void *)curr;
	}

	/*
	 * No free blocks are available.user_space hasn't caught up yet.
	 * Queue was just frozen and now this packet will get dropped.
	 */
	return NULL;
}