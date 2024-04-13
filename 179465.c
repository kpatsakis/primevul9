int compat_put_timeval(const struct timeval *tv, void __user *utv)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_to_user(utv, tv, sizeof(*tv)) ? -EFAULT : 0;
	else
		return __compat_put_timeval(tv, utv);
}