SYSCALL_DEFINE2(clock_getres, const clockid_t, which_clock,
		struct timespec __user *, tp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 rtn_tp64;
	struct timespec rtn_tp;
	int error;

	if (!kc)
		return -EINVAL;

	error = kc->clock_getres(which_clock, &rtn_tp64);
	rtn_tp = timespec64_to_timespec(rtn_tp64);

	if (!error && tp && copy_to_user(tp, &rtn_tp, sizeof (rtn_tp)))
		error = -EFAULT;

	return error;
}