static void *prb_dispatch_next_block(struct kbdq_core *pkc,
		struct packet_sock *po)
{
	struct block_desc *pbd;

	smp_rmb();

	/* 1. Get current block num */
	pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	/* 2. If this block is currently in_use then freeze the queue */
	if (TP_STATUS_USER & BLOCK_STATUS(pbd)) {
		prb_freeze_queue(pkc, po);
		return NULL;
	}

	/*
	 * 3.
	 * open this block and return the offset where the first packet
	 * needs to get stored.
	 */
	prb_open_block(pkc, pbd);
	return (void *)pkc->nxt_offset;
}