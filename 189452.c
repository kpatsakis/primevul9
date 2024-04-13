int jpc_fix_firstone(jpc_fix_t x)
{
	int n;

	/* The argument must be nonnegative. */
	assert(x >= 0);

	n = -1;
	while (x > 0) {
		x >>= 1;
		++n;
	}
	return n;
}