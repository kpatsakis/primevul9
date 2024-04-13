asmlinkage long compat_sys_select(int n, compat_ulong_t __user *inp,
	compat_ulong_t __user *outp, compat_ulong_t __user *exp,
	struct compat_timeval __user *tvp)
{
	s64 timeout = -1;
	struct compat_timeval tv;
	int ret;

	if (tvp) {
		if (copy_from_user(&tv, tvp, sizeof(tv)))
			return -EFAULT;

		if (tv.tv_sec < 0 || tv.tv_usec < 0)
			return -EINVAL;

		/* Cast to u64 to make GCC stop complaining */
		if ((u64)tv.tv_sec >= (u64)MAX_INT64_SECONDS)
			timeout = -1;	/* infinite */
		else {
			timeout = ROUND_UP(tv.tv_usec, 1000000/HZ);
			timeout += tv.tv_sec * HZ;
		}
	}

	ret = compat_core_sys_select(n, inp, outp, exp, &timeout);

	if (tvp) {
		struct compat_timeval rtv;

		if (current->personality & STICKY_TIMEOUTS)
			goto sticky;
		rtv.tv_usec = jiffies_to_usecs(do_div((*(u64*)&timeout), HZ));
		rtv.tv_sec = timeout;
		if (compat_timeval_compare(&rtv, &tv) >= 0)
			rtv = tv;
		if (copy_to_user(tvp, &rtv, sizeof(rtv))) {
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

	return ret;
}