int timer_reduce(struct timer_list *timer, unsigned long expires)
{
	return __mod_timer(timer, expires, MOD_TIMER_REDUCE);
}