static inline u64 add_gen(struct sdma_engine *sde, u64 qw1)
{
	u8 generation = (sde->descq_tail >> sde->sdma_shift) & 3;

	qw1 &= ~SDMA_DESC1_GENERATION_SMASK;
	qw1 |= ((u64)generation & SDMA_DESC1_GENERATION_MASK)
			<< SDMA_DESC1_GENERATION_SHIFT;
	return qw1;
}