static int __compat_get_timespec(struct timespec *ts, const struct old_timespec32 __user *cts)
{
	return (!access_ok(cts, sizeof(*cts)) ||
			__get_user(ts->tv_sec, &cts->tv_sec) ||
			__get_user(ts->tv_nsec, &cts->tv_nsec)) ? -EFAULT : 0;
}