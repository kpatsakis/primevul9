static void sdma_hw_clean_up_task(unsigned long opaque)
{
	struct sdma_engine *sde = (struct sdma_engine *)opaque;
	u64 statuscsr;

	while (1) {
#ifdef CONFIG_SDMA_VERBOSITY
		dd_dev_err(sde->dd, "CONFIG SDMA(%u) %s:%d %s()\n",
			   sde->this_idx, slashstrip(__FILE__), __LINE__,
			__func__);
#endif
		statuscsr = read_sde_csr(sde, SD(STATUS));
		statuscsr &= SD(STATUS_ENG_CLEANED_UP_SMASK);
		if (statuscsr)
			break;
		udelay(10);
	}

	sdma_process_event(sde, sdma_event_e25_hw_clean_up_done);
}