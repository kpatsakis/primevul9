static inline int timer_delete_hook(struct k_itimer *timer)
{
	const struct k_clock *kc = timer->kclock;

	if (WARN_ON_ONCE(!kc || !kc->timer_del))
		return -EINVAL;
	return kc->timer_del(timer);
}