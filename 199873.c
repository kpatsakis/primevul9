asmlinkage long compat_sys_pselect7(int n, compat_ulong_t __user *inp,
	compat_ulong_t __user *outp, compat_ulong_t __user *exp,
	struct compat_timespec __user *tsp, compat_sigset_t __user *sigmask,
	compat_size_t sigsetsize)
{
	compat_sigset_t ss32;
	sigset_t ksigmask, sigsaved;
	s64 timeout = MAX_SCHEDULE_TIMEOUT;
	struct compat_timespec ts;
	int ret;

	if (tsp) {
		if (copy_from_user(&ts, tsp, sizeof(ts)))
			return -EFAULT;

		if (ts.tv_sec < 0 || ts.tv_nsec < 0)
			return -EINVAL;
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

	do {
		if (tsp) {
			if ((unsigned long)ts.tv_sec < MAX_SELECT_SECONDS) {
				timeout = ROUND_UP(ts.tv_nsec, 1000000000/HZ);
				timeout += ts.tv_sec * (unsigned long)HZ;
				ts.tv_sec = 0;
				ts.tv_nsec = 0;
			} else {
				ts.tv_sec -= MAX_SELECT_SECONDS;
				timeout = MAX_SELECT_SECONDS * HZ;
			}
		}

		ret = compat_core_sys_select(n, inp, outp, exp, &timeout);

	} while (!ret && !timeout && tsp && (ts.tv_sec || ts.tv_nsec));

	if (tsp) {
		struct compat_timespec rts;

		if (current->personality & STICKY_TIMEOUTS)
			goto sticky;

		rts.tv_sec = timeout / HZ;
		rts.tv_nsec = (timeout % HZ) * (NSEC_PER_SEC/HZ);
		if (rts.tv_nsec >= NSEC_PER_SEC) {
			rts.tv_sec++;
			rts.tv_nsec -= NSEC_PER_SEC;
		}
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
			if (ret == -ERESTARTNOHAND)
				ret = -EINTR;
		}
	}

	if (ret == -ERESTARTNOHAND) {
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
	} else if (sigmask)
		sigprocmask(SIG_SETMASK, &sigsaved, NULL);

	return ret;
}