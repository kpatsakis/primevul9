void sdma_engine_interrupt(struct sdma_engine *sde, u64 status)
{
	trace_hfi1_sdma_engine_interrupt(sde, status);
	write_seqlock(&sde->head_lock);
	sdma_set_desc_cnt(sde, sdma_desct_intr);
	if (status & sde->idle_mask)
		sde->idle_int_cnt++;
	else if (status & sde->progress_mask)
		sde->progress_int_cnt++;
	else if (status & sde->int_mask)
		sde->sdma_int_cnt++;
	sdma_make_progress(sde, status);
	write_sequnlock(&sde->head_lock);
}