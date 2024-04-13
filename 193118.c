SYSCALL_DEFINE2(timer_gettime, timer_t, timer_id,
		struct itimerspec __user *, setting)
{
	struct itimerspec64 cur_setting64;
	struct itimerspec cur_setting;
	struct k_itimer *timr;
	const struct k_clock *kc;
	unsigned long flags;
	int ret = 0;

	timr = lock_timer(timer_id, &flags);
	if (!timr)
		return -EINVAL;

	memset(&cur_setting64, 0, sizeof(cur_setting64));
	kc = timr->kclock;
	if (WARN_ON_ONCE(!kc || !kc->timer_get))
		ret = -EINVAL;
	else
		kc->timer_get(timr, &cur_setting64);

	unlock_timer(timr, flags);

	cur_setting = itimerspec64_to_itimerspec(&cur_setting64);
	if (!ret && copy_to_user(setting, &cur_setting, sizeof (cur_setting)))
		return -EFAULT;

	return ret;
}