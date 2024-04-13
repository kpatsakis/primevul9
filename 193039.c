int put_compat_itimerspec(struct compat_itimerspec __user *dst,
			  const struct itimerspec *src)
{
	if (__compat_put_timespec(&src->it_interval, &dst->it_interval) ||
	    __compat_put_timespec(&src->it_value, &dst->it_value))
		return -EFAULT;
	return 0;
}