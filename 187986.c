static void prb_retire_current_block(struct kbdq_core *pkc,
		struct packet_sock *po, unsigned int status)
{
	struct block_desc *pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	/* retire/close the current block */
	if (likely(TP_STATUS_KERNEL == BLOCK_STATUS(pbd))) {
		/*
		 * Plug the case where copy_bits() is in progress on
		 * cpu-0 and tpacket_rcv() got invoked on cpu-1, didn't
		 * have space to copy the pkt in the current block and
		 * called prb_retire_current_block()
		 *
		 * We don't need to worry about the TMO case because
		 * the timer-handler already handled this case.
		 */
		if (!(status & TP_STATUS_BLK_TMO)) {
			while (atomic_read(&pkc->blk_fill_in_prog)) {
				/* Waiting for skb_copy_bits to finish... */
				cpu_relax();
			}
		}
		prb_close_block(pkc, pbd, po, status);
		return;
	}

	WARN(1, "ERROR-pbd[%d]:%p\n", pkc->kactive_blk_num, pbd);
	dump_stack();
	BUG();
}