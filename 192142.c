static void __mcheck_cpu_init_timer(void)
{
	struct timer_list *t = this_cpu_ptr(&mce_timer);

	timer_setup(t, mce_timer_fn, TIMER_PINNED);
	mce_start_timer(t);
}