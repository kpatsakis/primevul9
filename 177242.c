void sdma_wait(struct hfi1_devdata *dd)
{
	int i;

	for (i = 0; i < dd->num_sdma; i++) {
		struct sdma_engine *sde = &dd->per_sdma[i];

		sdma_wait_for_packet_egress(sde, 0);
	}
}