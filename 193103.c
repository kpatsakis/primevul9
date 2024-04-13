SYSCALL_DEFINE2(clock_adjtime, const clockid_t, which_clock,
		struct timex __user *, utx)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timex ktx;
	int err;

	if (!kc)
		return -EINVAL;
	if (!kc->clock_adj)
		return -EOPNOTSUPP;

	if (copy_from_user(&ktx, utx, sizeof(ktx)))
		return -EFAULT;

	err = kc->clock_adj(which_clock, &ktx);

	if (err >= 0 && copy_to_user(utx, &ktx, sizeof(ktx)))
		return -EFAULT;

	return err;
}