COMPAT_SYSCALL_DEFINE2(clock_adjtime, clockid_t, which_clock,
		       struct compat_timex __user *, utp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timex ktx;
	int err;

	if (!kc)
		return -EINVAL;
	if (!kc->clock_adj)
		return -EOPNOTSUPP;

	err = compat_get_timex(&ktx, utp);
	if (err)
		return err;

	err = kc->clock_adj(which_clock, &ktx);

	if (err >= 0)
		err = compat_put_timex(utp, &ktx);

	return err;
}