static int __init mcheck_late_init(void)
{
	if (mca_cfg.recovery)
		static_branch_inc(&mcsafe_key);

	mcheck_debugfs_init();
	cec_init();

	/*
	 * Flush out everything that has been logged during early boot, now that
	 * everything has been initialized (workqueues, decoders, ...).
	 */
	mce_schedule_work();

	return 0;
}