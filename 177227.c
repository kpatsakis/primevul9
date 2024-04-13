static inline u64 read_sde_csr(
	struct sdma_engine *sde,
	u32 offset0)
{
	return read_kctxt_csr(sde->dd, sde->this_idx, offset0);
}