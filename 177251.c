static inline void write_sde_csr(
	struct sdma_engine *sde,
	u32 offset0,
	u64 value)
{
	write_kctxt_csr(sde->dd, sde->this_idx, offset0, value);
}