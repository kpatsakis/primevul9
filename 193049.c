static ktime_t common_hrtimer_remaining(struct k_itimer *timr, ktime_t now)
{
	struct hrtimer *timer = &timr->it.real.timer;

	return __hrtimer_expires_remaining_adjusted(timer, now);
}