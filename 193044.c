COMPAT_SYSCALL_DEFINE2(clock_settime, clockid_t, which_clock,
		       struct compat_timespec __user *, tp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec ts;

	if (compat_get_timespec(&ts, tp))
		return -EFAULT;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_settime(which_clock,
				(struct timespec __user *) &ts);
	set_fs(oldfs);
	return err;
}