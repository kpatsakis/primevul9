void sdma_all_running(struct hfi1_devdata *dd)
{
	struct sdma_engine *sde;
	unsigned int i;

	/* move all engines to running */
	for (i = 0; i < dd->num_sdma; ++i) {
		sde = &dd->per_sdma[i];
		sdma_process_event(sde, sdma_event_e30_go_running);
	}
}