void mce_timer_kick(unsigned long interval)
{
	struct timer_list *t = this_cpu_ptr(&mce_timer);
	unsigned long iv = __this_cpu_read(mce_next_interval);

	__start_timer(t, interval);

	if (interval < iv)
		__this_cpu_write(mce_next_interval, interval);
}