SYSCALL_DEFINE2(clock_gettime, const clockid_t, which_clock,
		struct timespec __user *,tp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 kernel_tp64;
	struct timespec kernel_tp;
	int error;

	if (!kc)
		return -EINVAL;

	error = kc->clock_get(which_clock, &kernel_tp64);
	kernel_tp = timespec64_to_timespec(kernel_tp64);

	if (!error && copy_to_user(tp, &kernel_tp, sizeof (kernel_tp)))
		error = -EFAULT;

	return error;
}