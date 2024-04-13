static int __compat_put_timespec(const struct timespec *ts, struct old_timespec32 __user *cts)
{
	return (!access_ok(cts, sizeof(*cts)) ||
			__put_user(ts->tv_sec, &cts->tv_sec) ||
			__put_user(ts->tv_nsec, &cts->tv_nsec)) ? -EFAULT : 0;
}