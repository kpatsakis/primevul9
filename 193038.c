COMPAT_SYSCALL_DEFINE2(getitimer, int, which,
		struct compat_itimerval __user *, it)
{
	struct itimerval kit;
	int error;

	if (!IS_ENABLED(CONFIG_POSIX_TIMERS))
		return sys_ni_posix_timers();

	error = do_getitimer(which, &kit);
	if (!error && put_compat_itimerval(it, &kit))
		error = -EFAULT;
	return error;
}