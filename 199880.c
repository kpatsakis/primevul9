asmlinkage long compat_sys_ppoll(struct pollfd __user *ufds,
	unsigned int nfds, struct compat_timespec __user *tsp,
	const compat_sigset_t __user *sigmask, compat_size_t sigsetsize)
{
	compat_sigset_t ss32;
	sigset_t ksigmask, sigsaved;
	struct compat_timespec ts;
	s64 timeout = -1;
	int ret;

	if (tsp) {
		if (copy_from_user(&ts, tsp, sizeof(ts)))
			return -EFAULT;

		/* We assume that ts.tv_sec is always lower than
		   the number of seconds that can be expressed in
		   an s64. Otherwise the compiler bitches at us */
		timeout = ROUND_UP(ts.tv_nsec, 1000000000/HZ);
		timeout += ts.tv_sec * HZ;
	}

	if (sigmask) {
		if (sigsetsize != sizeof(compat_sigset_t))
			return -EINVAL;
		if (copy_from_user(&ss32, sigmask, sizeof(ss32)))
			return -EFAULT;
		sigset_from_compat(&ksigmask, &ss32);

		sigdelsetmask(&ksigmask, sigmask(SIGKILL)|sigmask(SIGSTOP));
		sigprocmask(SIG_SETMASK, &ksigmask, &sigsaved);
	}

	ret = do_sys_poll(ufds, nfds, &timeout);

	/* We can restart this syscall, usually */
	if (ret == -EINTR) {
		/*
		 * Don't restore the signal mask yet. Let do_signal() deliver
		 * the signal on the way back to userspace, before the signal
		 * mask is restored.
		 */
		if (sigmask) {
			memcpy(&current->saved_sigmask, &sigsaved,
				sizeof(sigsaved));
			set_thread_flag(TIF_RESTORE_SIGMASK);
		}
		ret = -ERESTARTNOHAND;
	} else if (sigmask)
		sigprocmask(SIG_SETMASK, &sigsaved, NULL);

	if (tsp && timeout >= 0) {
		struct compat_timespec rts;

		if (current->personality & STICKY_TIMEOUTS)
			goto sticky;
		/* Yes, we know it's actually an s64, but it's also positive. */
		rts.tv_nsec = jiffies_to_usecs(do_div((*(u64*)&timeout), HZ)) *
					1000;
		rts.tv_sec = timeout;
		if (compat_timespec_compare(&rts, &ts) >= 0)
			rts = ts;
		if (copy_to_user(tsp, &rts, sizeof(rts))) {
sticky:
			/*
			 * If an application puts its timeval in read-only
			 * memory, we don't want the Linux-specific update to
			 * the timeval to cause a fault after the select has
			 * completed successfully. However, because we're not
			 * updating the timeval, we can't restart the system
			 * call.
			 */
			if (ret == -ERESTARTNOHAND && timeout >= 0)
				ret = -EINTR;
		}
	}

	return ret;
}