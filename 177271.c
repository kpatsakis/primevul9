void sdma_engine_error(struct sdma_engine *sde, u64 status)
{
	unsigned long flags;

#ifdef CONFIG_SDMA_VERBOSITY
	dd_dev_err(sde->dd, "CONFIG SDMA(%u) error status 0x%llx state %s\n",
		   sde->this_idx,
		   (unsigned long long)status,
		   sdma_state_names[sde->state.current_state]);
#endif
	spin_lock_irqsave(&sde->tail_lock, flags);
	write_seqlock(&sde->head_lock);
	if (status & ALL_SDMA_ENG_HALT_ERRS)
		__sdma_process_event(sde, sdma_event_e60_hw_halted);
	if (status & ~SD(ENG_ERR_STATUS_SDMA_HALT_ERR_SMASK)) {
		dd_dev_err(sde->dd,
			   "SDMA (%u) engine error: 0x%llx state %s\n",
			   sde->this_idx,
			   (unsigned long long)status,
			   sdma_state_names[sde->state.current_state]);
		dump_sdma_state(sde);
	}
	write_sequnlock(&sde->head_lock);
	spin_unlock_irqrestore(&sde->tail_lock, flags);
}