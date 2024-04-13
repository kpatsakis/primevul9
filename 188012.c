static void prb_del_retire_blk_timer(struct kbdq_core *pkc)
{
	del_timer_sync(&pkc->retire_blk_timer);
}