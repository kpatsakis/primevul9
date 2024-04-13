static int mce_end(int order)
{
	int ret = -1;
	u64 timeout = (u64)mca_cfg.monarch_timeout * NSEC_PER_USEC;

	if (!timeout)
		goto reset;
	if (order < 0)
		goto reset;

	/*
	 * Allow others to run.
	 */
	atomic_inc(&mce_executing);

	if (order == 1) {
		/* CHECKME: Can this race with a parallel hotplug? */
		int cpus = num_online_cpus();

		/*
		 * Monarch: Wait for everyone to go through their scanning
		 * loops.
		 */
		while (atomic_read(&mce_executing) <= cpus) {
			if (mce_timed_out(&timeout,
					  "Timeout: Monarch CPU unable to finish machine check processing"))
				goto reset;
			ndelay(SPINUNIT);
		}

		mce_reign();
		barrier();
		ret = 0;
	} else {
		/*
		 * Subject: Wait for Monarch to finish.
		 */
		while (atomic_read(&mce_executing) != 0) {
			if (mce_timed_out(&timeout,
					  "Timeout: Monarch CPU did not finish machine check processing"))
				goto reset;
			ndelay(SPINUNIT);
		}

		/*
		 * Don't reset anything. That's done by the Monarch.
		 */
		return 0;
	}

	/*
	 * Reset all global state.
	 */
reset:
	atomic_set(&global_nwo, 0);
	atomic_set(&mce_callin, 0);
	barrier();

	/*
	 * Let others run again.
	 */
	atomic_set(&mce_executing, 0);
	return ret;
}