static int mce_start(int *no_way_out)
{
	int order;
	int cpus = num_online_cpus();
	u64 timeout = (u64)mca_cfg.monarch_timeout * NSEC_PER_USEC;

	if (!timeout)
		return -1;

	atomic_add(*no_way_out, &global_nwo);
	/*
	 * Rely on the implied barrier below, such that global_nwo
	 * is updated before mce_callin.
	 */
	order = atomic_inc_return(&mce_callin);

	/*
	 * Wait for everyone.
	 */
	while (atomic_read(&mce_callin) != cpus) {
		if (mce_timed_out(&timeout,
				  "Timeout: Not all CPUs entered broadcast exception handler")) {
			atomic_set(&global_nwo, 0);
			return -1;
		}
		ndelay(SPINUNIT);
	}

	/*
	 * mce_callin should be read before global_nwo
	 */
	smp_rmb();

	if (order == 1) {
		/*
		 * Monarch: Starts executing now, the others wait.
		 */
		atomic_set(&mce_executing, 1);
	} else {
		/*
		 * Subject: Now start the scanning loop one by one in
		 * the original callin order.
		 * This way when there are any shared banks it will be
		 * only seen by one CPU before cleared, avoiding duplicates.
		 */
		while (atomic_read(&mce_executing) < order) {
			if (mce_timed_out(&timeout,
					  "Timeout: Subject CPUs unable to finish machine check processing")) {
				atomic_set(&global_nwo, 0);
				return -1;
			}
			ndelay(SPINUNIT);
		}
	}

	/*
	 * Cache the global no_way_out state.
	 */
	*no_way_out = atomic_read(&global_nwo);

	return order;
}