COMPAT_SYSCALL_DEFINE4(timer_settime, timer_t, timer_id, int, flags,
		       struct compat_itimerspec __user *, new,
		       struct compat_itimerspec __user *, old)
{
	long err;
	mm_segment_t oldfs;
	struct itimerspec newts, oldts;

	if (!new)
		return -EINVAL;
	if (get_compat_itimerspec(&newts, new))
		return -EFAULT;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_timer_settime(timer_id, flags,
				(struct itimerspec __user *) &newts,
				(struct itimerspec __user *) &oldts);
	set_fs(oldfs);
	if (!err && old && put_compat_itimerspec(old, &oldts))
		return -EFAULT;
	return err;
}