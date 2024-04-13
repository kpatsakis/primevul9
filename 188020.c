static inline int prb_queue_frozen(struct kbdq_core *pkc)
{
	return pkc->reset_pending_on_curr_blk;
}