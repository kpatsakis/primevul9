int compat_get_timespec(struct timespec *ts, const void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_from_user(ts, uts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_get_timespec(ts, uts);
}