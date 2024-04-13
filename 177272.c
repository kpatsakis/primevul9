static inline void sdma_set_desc_cnt(struct sdma_engine *sde, unsigned cnt)
{
	u64 reg;

	if (!(sde->dd->flags & HFI1_HAS_SDMA_TIMEOUT))
		return;
	reg = cnt;
	reg &= SD(DESC_CNT_CNT_MASK);
	reg <<= SD(DESC_CNT_CNT_SHIFT);
	write_sde_csr(sde, SD(DESC_CNT), reg);
}