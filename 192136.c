static int mce_default_notifier(struct notifier_block *nb, unsigned long val,
				void *data)
{
	struct mce *m = (struct mce *)data;

	if (!m)
		return NOTIFY_DONE;

	if (atomic_read(&num_notifiers) > NUM_DEFAULT_NOTIFIERS)
		return NOTIFY_DONE;

	__print_mce(m);

	return NOTIFY_DONE;
}