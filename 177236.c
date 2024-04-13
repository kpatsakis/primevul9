void sdma_ahg_free(struct sdma_engine *sde, int ahg_index)
{
	if (!sde)
		return;
	trace_hfi1_ahg_deallocate(sde, ahg_index);
	if (ahg_index < 0 || ahg_index > 31)
		return;
	clear_bit(ahg_index, &sde->ahg_bits);
}