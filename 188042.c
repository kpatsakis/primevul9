static inline void prb_thaw_queue(struct kbdq_core *pkc)
{
	pkc->reset_pending_on_curr_blk = 0;
}