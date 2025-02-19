static inline jas_ulong encode_twos_comp(long n, int prec)
{
	jas_ulong result;
	assert(prec >= 2);
	jas_eprintf("warning: support for signed data is untested\n");
	// NOTE: Is this correct?
	if (n < 0) {
		result = -n;
		result = (result ^ 0xffffffffUL) + 1;
		result &= (1 << prec) - 1;
	} else {
		result = n;
	}
	return result;
}