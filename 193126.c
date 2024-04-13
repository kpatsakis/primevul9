int common_timer_del(struct k_itimer *timer)
{
	const struct k_clock *kc = timer->kclock;

	timer->it_interval = 0;
	if (kc->timer_try_to_cancel(timer) < 0)
		return TIMER_RETRY;
	timer->it_active = 0;
	return 0;
}