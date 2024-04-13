void sdma_all_idle(struct hfi1_devdata *dd)
{
	struct sdma_engine *sde;
	unsigned int i;

	/* idle all engines */
	for (i = 0; i < dd->num_sdma; ++i) {
		sde = &dd->per_sdma[i];
		sdma_process_event(sde, sdma_event_e70_go_idle);
	}
}