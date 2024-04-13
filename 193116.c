COMPAT_SYSCALL_DEFINE3(setitimer, int, which,
		struct compat_itimerval __user *, in,
		struct compat_itimerval __user *, out)
{
	struct itimerval kin, kout;
	int error;

	if (!IS_ENABLED(CONFIG_POSIX_TIMERS))
		return sys_ni_posix_timers();

	if (in) {
		if (get_compat_itimerval(&kin, in))
			return -EFAULT;
	} else
		memset(&kin, 0, sizeof(kin));

	error = do_setitimer(which, &kin, out ? &kout : NULL);
	if (error || !out)
		return error;
	if (put_compat_itimerval(out, &kout))
		return -EFAULT;
	return 0;
}