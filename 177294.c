struct sdma_engine *sdma_select_engine_sc(
	struct hfi1_devdata *dd,
	u32 selector,
	u8 sc5)
{
	u8 vl = sc_to_vlt(dd, sc5);

	return sdma_select_engine_vl(dd, selector, vl);
}