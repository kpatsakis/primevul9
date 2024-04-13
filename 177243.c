void _sdma_engine_progress_schedule(
	struct sdma_engine *sde)
{
	trace_hfi1_sdma_engine_progress(sde, sde->progress_mask);
	/* assume we have selected a good cpu */
	write_csr(sde->dd,
		  CCE_INT_FORCE + (8 * (IS_SDMA_START / 64)),
		  sde->progress_mask);
}