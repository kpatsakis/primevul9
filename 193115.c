int compat_convert_timespec(struct timespec __user **kts,
			    const void __user *cts)
{
	struct timespec ts;
	struct timespec __user *uts;

	if (!cts || COMPAT_USE_64BIT_TIME) {
		*kts = (struct timespec __user *)cts;
		return 0;
	}

	uts = compat_alloc_user_space(sizeof(ts));
	if (!uts)
		return -EFAULT;
	if (compat_get_timespec(&ts, cts))
		return -EFAULT;
	if (copy_to_user(uts, &ts, sizeof(ts)))
		return -EFAULT;

	*kts = uts;
	return 0;
}