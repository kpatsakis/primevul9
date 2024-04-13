static inline int prb_curr_blk_in_use(struct kbdq_core *pkc,
				      struct block_desc *pbd)
{
	return TP_STATUS_USER & BLOCK_STATUS(pbd);
}