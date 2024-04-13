SYSCALL_DEFINE4(clock_nanosleep, const clockid_t, which_clock, int, flags,
		const struct timespec __user *, rqtp,
		struct timespec __user *, rmtp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 t64;
	struct timespec t;

	if (!kc)
		return -EINVAL;
	if (!kc->nsleep)
		return -ENANOSLEEP_NOTSUP;

	if (copy_from_user(&t, rqtp, sizeof (struct timespec)))
		return -EFAULT;

	t64 = timespec_to_timespec64(t);
	if (!timespec64_valid(&t64))
		return -EINVAL;
	if (flags & TIMER_ABSTIME)
		rmtp = NULL;
	current->restart_block.nanosleep.type = rmtp ? TT_NATIVE : TT_NONE;
	current->restart_block.nanosleep.rmtp = rmtp;

	return kc->nsleep(which_clock, flags, &t64);
}