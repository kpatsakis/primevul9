COMPAT_SYSCALL_DEFINE2(settimeofday, struct compat_timeval __user *, tv,
		       struct timezone __user *, tz)
{
	struct timespec64 new_ts;
	struct timeval user_tv;
	struct timezone new_tz;

	if (tv) {
		if (compat_get_timeval(&user_tv, tv))
			return -EFAULT;
		new_ts.tv_sec = user_tv.tv_sec;
		new_ts.tv_nsec = user_tv.tv_usec * NSEC_PER_USEC;
	}
	if (tz) {
		if (copy_from_user(&new_tz, tz, sizeof(*tz)))
			return -EFAULT;
	}

	return do_sys_settimeofday64(tv ? &new_ts : NULL, tz ? &new_tz : NULL);
}