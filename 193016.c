COMPAT_SYSCALL_DEFINE4(rt_sigtimedwait, compat_sigset_t __user *, uthese,
		struct compat_siginfo __user *, uinfo,
		struct compat_timespec __user *, uts, compat_size_t, sigsetsize)
{
	compat_sigset_t s32;
	sigset_t s;
	struct timespec t;
	siginfo_t info;
	long ret;

	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&s32, uthese, sizeof(compat_sigset_t)))
		return -EFAULT;
	sigset_from_compat(&s, &s32);

	if (uts) {
		if (compat_get_timespec(&t, uts))
			return -EFAULT;
	}

	ret = do_sigtimedwait(&s, &info, uts ? &t : NULL);

	if (ret > 0 && uinfo) {
		if (copy_siginfo_to_user32(uinfo, &info))
			ret = -EFAULT;
	}

	return ret;
}