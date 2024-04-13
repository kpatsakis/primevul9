COMPAT_SYSCALL_DEFINE1(time, compat_time_t __user *, tloc)
{
	compat_time_t i;
	struct timeval tv;

	do_gettimeofday(&tv);
	i = tv.tv_sec;

	if (tloc) {
		if (put_user(i,tloc))
			return -EFAULT;
	}
	force_successful_syscall_return();
	return i;
}