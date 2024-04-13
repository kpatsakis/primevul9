COMPAT_SYSCALL_DEFINE3(sched_setaffinity, compat_pid_t, pid,
		       unsigned int, len,
		       compat_ulong_t __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = compat_get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval)
		goto out;

	retval = sched_setaffinity(pid, new_mask);
out:
	free_cpumask_var(new_mask);
	return retval;
}