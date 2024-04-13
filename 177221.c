static void sdma_err_halt_wait(struct work_struct *work)
{
	struct sdma_engine *sde = container_of(work, struct sdma_engine,
						err_halt_worker);
	u64 statuscsr;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(SDMA_ERR_HALT_TIMEOUT);
	while (1) {
		statuscsr = read_sde_csr(sde, SD(STATUS));
		statuscsr &= SD(STATUS_ENG_HALTED_SMASK);
		if (statuscsr)
			break;
		if (time_after(jiffies, timeout)) {
			dd_dev_err(sde->dd,
				   "SDMA engine %d - timeout waiting for engine to halt\n",
				   sde->this_idx);
			/*
			 * Continue anyway.  This could happen if there was
			 * an uncorrectable error in the wrong spot.
			 */
			break;
		}
		usleep_range(80, 120);
	}

	sdma_process_event(sde, sdma_event_e15_hw_halt_done);
}