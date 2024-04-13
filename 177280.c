static void set_sdma_integrity(struct sdma_engine *sde)
{
	struct hfi1_devdata *dd = sde->dd;

	write_sde_csr(sde, SD(CHECK_ENABLE),
		      hfi1_pkt_base_sdma_integrity(dd));
}