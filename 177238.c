void sdma_unfreeze(struct hfi1_devdata *dd)
{
	int i;

	/* tell all engines start freeze clean up */
	for (i = 0; i < dd->num_sdma; i++)
		sdma_process_event(&dd->per_sdma[i],
				   sdma_event_e82_hw_unfreeze);
}