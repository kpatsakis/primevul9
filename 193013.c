static int __compat_put_timespec(const struct timespec *ts, struct compat_timespec __user *cts)
{
	return (!access_ok(VERIFY_WRITE, cts, sizeof(*cts)) ||
			__put_user(ts->tv_sec, &cts->tv_sec) ||
			__put_user(ts->tv_nsec, &cts->tv_nsec)) ? -EFAULT : 0;
}