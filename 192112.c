int mce_notify_irq(void)
{
	/* Not more than two messages every minute */
	static DEFINE_RATELIMIT_STATE(ratelimit, 60*HZ, 2);

	if (test_and_clear_bit(0, &mce_need_notify)) {
		mce_work_trigger();

		if (__ratelimit(&ratelimit))
			pr_info(HW_ERR "Machine check events logged\n");

		return 1;
	}
	return 0;
}