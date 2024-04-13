COMPAT_SYSCALL_DEFINE2(sched_rr_get_interval,
		       compat_pid_t, pid,
		       struct compat_timespec __user *, interval)
{
	struct timespec t;
	int ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_sched_rr_get_interval(pid, (struct timespec __user *)&t);
	set_fs(old_fs);
	if (compat_put_timespec(&t, interval))
		return -EFAULT;
	return ret;
}