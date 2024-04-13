COMPAT_SYSCALL_DEFINE2(gettimeofday, struct compat_timeval __user *, tv,
		       struct timezone __user *, tz)
{
	if (tv) {
		struct timeval ktv;
		do_gettimeofday(&ktv);
		if (compat_put_timeval(&ktv, tv))
			return -EFAULT;
	}
	if (tz) {
		if (copy_to_user(tz, &sys_tz, sizeof(sys_tz)))
			return -EFAULT;
	}

	return 0;
}