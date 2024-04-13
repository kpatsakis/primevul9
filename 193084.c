int get_compat_itimerspec(struct itimerspec *dst,
			  const struct compat_itimerspec __user *src)
{
	if (__compat_get_timespec(&dst->it_interval, &src->it_interval) ||
	    __compat_get_timespec(&dst->it_value, &src->it_value))
		return -EFAULT;
	return 0;
}