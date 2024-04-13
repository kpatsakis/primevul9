COMPAT_SYSCALL_DEFINE4(wait4,
	compat_pid_t, pid,
	compat_uint_t __user *, stat_addr,
	int, options,
	struct compat_rusage __user *, ru)
{
	if (!ru) {
		return sys_wait4(pid, stat_addr, options, NULL);
	} else {
		struct rusage r;
		int ret;
		unsigned int status;
		mm_segment_t old_fs = get_fs();

		set_fs (KERNEL_DS);
		ret = sys_wait4(pid,
				(stat_addr ?
				 (unsigned int __user *) &status : NULL),
				options, (struct rusage __user *) &r);
		set_fs (old_fs);

		if (ret > 0) {
			if (put_compat_rusage(&r, ru))
				return -EFAULT;
			if (stat_addr && put_user(status, stat_addr))
				return -EFAULT;
		}
		return ret;
	}
}