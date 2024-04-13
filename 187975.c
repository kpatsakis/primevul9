static void prb_init_blk_timer(struct packet_sock *po,
		struct kbdq_core *pkc,
		void (*func) (unsigned long))
{
	init_timer(&pkc->retire_blk_timer);
	pkc->retire_blk_timer.data = (long)po;
	pkc->retire_blk_timer.function = func;
	pkc->retire_blk_timer.expires = jiffies;
}