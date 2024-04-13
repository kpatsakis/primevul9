int compat_get_timeval(struct timeval *tv, const void __user *utv)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_from_user(tv, utv, sizeof(*tv)) ? -EFAULT : 0;
	else
		return __compat_get_timeval(tv, utv);
}