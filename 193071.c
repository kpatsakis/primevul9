COMPAT_SYSCALL_DEFINE2(timer_gettime, timer_t, timer_id,
		       struct compat_itimerspec __user *, setting)
{
	long err;
	mm_segment_t oldfs;
	struct itimerspec ts;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_timer_gettime(timer_id,
				(struct itimerspec __user *) &ts);
	set_fs(oldfs);
	if (!err && put_compat_itimerspec(setting, &ts))
		return -EFAULT;
	return err;
}