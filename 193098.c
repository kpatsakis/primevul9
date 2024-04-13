static int common_timer_create(struct k_itimer *new_timer)
{
	hrtimer_init(&new_timer->it.real.timer, new_timer->it_clock, 0);
	return 0;
}