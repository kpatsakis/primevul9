static int common_hrtimer_forward(struct k_itimer *timr, ktime_t now)
{
	struct hrtimer *timer = &timr->it.real.timer;

	return (int)hrtimer_forward(timer, now, timr->it_interval);
}