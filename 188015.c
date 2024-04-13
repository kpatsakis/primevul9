static inline void prb_fill_curr_block(char *curr, struct kbdq_core *pkc,
				struct block_desc *pbd,
				unsigned int len)
{
	struct tpacket3_hdr *ppd;

	ppd  = (struct tpacket3_hdr *)curr;
	ppd->tp_next_offset = TOTAL_PKT_LEN_INCL_ALIGN(len);
	pkc->prev = curr;
	pkc->nxt_offset += TOTAL_PKT_LEN_INCL_ALIGN(len);
	BLOCK_LEN(pbd) += TOTAL_PKT_LEN_INCL_ALIGN(len);
	BLOCK_NUM_PKTS(pbd) += 1;
	atomic_inc(&pkc->blk_fill_in_prog);
	prb_run_all_ft_ops(pkc, ppd);
}