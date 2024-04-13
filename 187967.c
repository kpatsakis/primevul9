static void prb_close_block(struct kbdq_core *pkc1, struct block_desc *pbd1,
		struct packet_sock *po, unsigned int stat)
{
	__u32 status = TP_STATUS_USER | stat;

	struct tpacket3_hdr *last_pkt;
	struct hdr_v1 *h1 = &pbd1->hdr.bh1;

	if (po->stats.tp_drops)
		status |= TP_STATUS_LOSING;

	last_pkt = (struct tpacket3_hdr *)pkc1->prev;
	last_pkt->tp_next_offset = 0;

	/* Get the ts of the last pkt */
	if (BLOCK_NUM_PKTS(pbd1)) {
		h1->ts_last_pkt.ts_sec = last_pkt->tp_sec;
		h1->ts_last_pkt.ts_nsec	= last_pkt->tp_nsec;
	} else {
		/* Ok, we tmo'd - so get the current time */
		struct timespec ts;
		getnstimeofday(&ts);
		h1->ts_last_pkt.ts_sec = ts.tv_sec;
		h1->ts_last_pkt.ts_nsec	= ts.tv_nsec;
	}

	smp_wmb();

	/* Flush the block */
	prb_flush_block(pkc1, pbd1, status);

	pkc1->kactive_blk_num = GET_NEXT_PRB_BLK_NUM(pkc1);
}