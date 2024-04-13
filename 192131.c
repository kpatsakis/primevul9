static int mce_first_notifier(struct notifier_block *nb, unsigned long val,
			      void *data)
{
	struct mce *m = (struct mce *)data;

	if (!m)
		return NOTIFY_DONE;

	if (cec_add_mce(m))
		return NOTIFY_STOP;

	/* Emit the trace record: */
	trace_mce_record(m);

	set_bit(0, &mce_need_notify);

	mce_notify_irq();

	return NOTIFY_DONE;
}