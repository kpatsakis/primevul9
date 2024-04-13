void sdma_freeze(struct hfi1_devdata *dd)
{
	int i;
	int ret;

	/*
	 * Make sure all engines have moved out of the running state before
	 * continuing.
	 */
	ret = wait_event_interruptible(dd->sdma_unfreeze_wq,
				       atomic_read(&dd->sdma_unfreeze_count) <=
				       0);
	/* interrupted or count is negative, then unloading - just exit */
	if (ret || atomic_read(&dd->sdma_unfreeze_count) < 0)
		return;

	/* set up the count for the next wait */
	atomic_set(&dd->sdma_unfreeze_count, dd->num_sdma);

	/* tell all engines that the SPC is frozen, they can start cleaning */
	for (i = 0; i < dd->num_sdma; i++)
		sdma_process_event(&dd->per_sdma[i], sdma_event_e81_hw_frozen);

	/*
	 * Wait for everyone to finish software clean before exiting.  The
	 * software clean will read engine CSRs, so must be completed before
	 * the next step, which will clear the engine CSRs.
	 */
	(void)wait_event_interruptible(dd->sdma_unfreeze_wq,
				atomic_read(&dd->sdma_unfreeze_count) <= 0);
	/* no need to check results - done no matter what */
}