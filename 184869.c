static inline long decode_twos_comp(jas_ulong c, int prec)
{
	long result;
	assert(prec >= 2);
	jas_eprintf("warning: support for signed data is untested\n");
	// NOTE: Is this correct?
	result = (c & ((1 << (prec - 1)) - 1)) - (c & (1 << (prec - 1)));
	return result;
}