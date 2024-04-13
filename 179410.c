int compat_put_timespec(const struct timespec *ts, void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_to_user(uts, ts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_put_timespec(ts, uts);
}