void sdma_start(struct hfi1_devdata *dd)
{
	unsigned i;
	struct sdma_engine *sde;

	/* kick off the engines state processing */
	for (i = 0; i < dd->num_sdma; ++i) {
		sde = &dd->per_sdma[i];
		sdma_process_event(sde, sdma_event_e10_go_hw_start);
	}
}