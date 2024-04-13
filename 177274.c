void sdma_update_lmc(struct hfi1_devdata *dd, u64 mask, u32 lid)
{
	struct sdma_engine *sde;
	int i;
	u64 sreg;

	sreg = ((mask & SD(CHECK_SLID_MASK_MASK)) <<
		SD(CHECK_SLID_MASK_SHIFT)) |
		(((lid & mask) & SD(CHECK_SLID_VALUE_MASK)) <<
		SD(CHECK_SLID_VALUE_SHIFT));

	for (i = 0; i < dd->num_sdma; i++) {
		hfi1_cdbg(LINKVERB, "SendDmaEngine[%d].SLID_CHECK = 0x%x",
			  i, (u32)sreg);
		sde = &dd->per_sdma[i];
		write_sde_csr(sde, SD(CHECK_SLID), sreg);
	}
}