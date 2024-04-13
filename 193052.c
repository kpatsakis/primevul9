COMPAT_SYSCALL_DEFINE2(clock_gettime, clockid_t, which_clock,
		       struct compat_timespec __user *, tp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec ts;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_gettime(which_clock,
				(struct timespec __user *) &ts);
	set_fs(oldfs);
	if (!err && compat_put_timespec(&ts, tp))
		return -EFAULT;
	return err;
}