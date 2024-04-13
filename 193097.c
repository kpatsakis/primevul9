SYSCALL_DEFINE2(clock_settime, const clockid_t, which_clock,
		const struct timespec __user *, tp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 new_tp64;
	struct timespec new_tp;

	if (!kc || !kc->clock_set)
		return -EINVAL;

	if (copy_from_user(&new_tp, tp, sizeof (*tp)))
		return -EFAULT;
	new_tp64 = timespec_to_timespec64(new_tp);

	return kc->clock_set(which_clock, &new_tp64);
}