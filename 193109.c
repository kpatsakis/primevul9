static int common_hrtimer_try_to_cancel(struct k_itimer *timr)
{
	return hrtimer_try_to_cancel(&timr->it.real.timer);
}