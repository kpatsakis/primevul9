COMPAT_SYSCALL_DEFINE5(waitid,
		int, which, compat_pid_t, pid,
		struct compat_siginfo __user *, uinfo, int, options,
		struct compat_rusage __user *, uru)
{
	siginfo_t info;
	struct rusage ru;
	long ret;
	mm_segment_t old_fs = get_fs();

	memset(&info, 0, sizeof(info));

	set_fs(KERNEL_DS);
	ret = sys_waitid(which, pid, (siginfo_t __user *)&info, options,
			 uru ? (struct rusage __user *)&ru : NULL);
	set_fs(old_fs);

	if ((ret < 0) || (info.si_signo == 0))
		return ret;

	if (uru) {
		/* sys_waitid() overwrites everything in ru */
		if (COMPAT_USE_64BIT_TIME)
			ret = copy_to_user(uru, &ru, sizeof(ru));
		else
			ret = put_compat_rusage(&ru, uru);
		if (ret)
			return -EFAULT;
	}

	BUG_ON(info.si_code & __SI_MASK);
	info.si_code |= __SI_CHLD;
	return copy_siginfo_to_user32(uinfo, &info);
}