COMPAT_SYSCALL_DEFINE1(sigpending, compat_old_sigset_t __user *, set)
{
	old_sigset_t s;
	long ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_sigpending((old_sigset_t __user *) &s);
	set_fs(old_fs);
	if (ret == 0)
		ret = put_user(s, set);
	return ret;
}