static void mce_start_timer(struct timer_list *t)
{
	unsigned long iv = check_interval * HZ;

	if (mca_cfg.ignore_ce || !iv)
		return;

	this_cpu_write(mce_next_interval, iv);
	__start_timer(t, iv);
}