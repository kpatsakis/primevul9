int sdma_ahg_alloc(struct sdma_engine *sde)
{
	int nr;
	int oldbit;

	if (!sde) {
		trace_hfi1_ahg_allocate(sde, -EINVAL);
		return -EINVAL;
	}
	while (1) {
		nr = ffz(READ_ONCE(sde->ahg_bits));
		if (nr > 31) {
			trace_hfi1_ahg_allocate(sde, -ENOSPC);
			return -ENOSPC;
		}
		oldbit = test_and_set_bit(nr, &sde->ahg_bits);
		if (!oldbit)
			break;
		cpu_relax();
	}
	trace_hfi1_ahg_allocate(sde, nr);
	return nr;
}