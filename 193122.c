SYSCALL_DEFINE4(timer_settime, timer_t, timer_id, int, flags,
		const struct itimerspec __user *, new_setting,
		struct itimerspec __user *, old_setting)
{
	struct itimerspec64 new_spec64, old_spec64;
	struct itimerspec64 *rtn = old_setting ? &old_spec64 : NULL;
	struct itimerspec new_spec, old_spec;
	struct k_itimer *timr;
	unsigned long flag;
	const struct k_clock *kc;
	int error = 0;

	if (!new_setting)
		return -EINVAL;

	if (copy_from_user(&new_spec, new_setting, sizeof (new_spec)))
		return -EFAULT;
	new_spec64 = itimerspec_to_itimerspec64(&new_spec);

	if (!timespec64_valid(&new_spec64.it_interval) ||
	    !timespec64_valid(&new_spec64.it_value))
		return -EINVAL;
	if (rtn)
		memset(rtn, 0, sizeof(*rtn));
retry:
	timr = lock_timer(timer_id, &flag);
	if (!timr)
		return -EINVAL;

	kc = timr->kclock;
	if (WARN_ON_ONCE(!kc || !kc->timer_set))
		error = -EINVAL;
	else
		error = kc->timer_set(timr, flags, &new_spec64, rtn);

	unlock_timer(timr, flag);
	if (error == TIMER_RETRY) {
		rtn = NULL;	// We already got the old time...
		goto retry;
	}

	old_spec = itimerspec64_to_itimerspec(&old_spec64);
	if (old_setting && !error &&
	    copy_to_user(old_setting, &old_spec, sizeof (old_spec)))
		error = -EFAULT;

	return error;
}