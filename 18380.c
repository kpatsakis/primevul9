int mod_timer(struct timer_list *timer, unsigned long expires)
{
	return __mod_timer(timer, expires, 0);
}