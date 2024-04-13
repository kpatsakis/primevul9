COMPAT_SYSCALL_DEFINE2(old_getrlimit, unsigned int, resource,
		       struct compat_rlimit __user *, rlim)
{
	struct rlimit r;
	int ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_old_getrlimit(resource, (struct rlimit __user *)&r);
	set_fs(old_fs);

	if (!ret) {
		if (r.rlim_cur > COMPAT_RLIM_OLD_INFINITY)
			r.rlim_cur = COMPAT_RLIM_INFINITY;
		if (r.rlim_max > COMPAT_RLIM_OLD_INFINITY)
			r.rlim_max = COMPAT_RLIM_INFINITY;

		if (!access_ok(VERIFY_WRITE, rlim, sizeof(*rlim)) ||
		    __put_user(r.rlim_cur, &rlim->rlim_cur) ||
		    __put_user(r.rlim_max, &rlim->rlim_max))
			return -EFAULT;
	}
	return ret;
}