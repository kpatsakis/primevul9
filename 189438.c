static int jpc_putcommacode(jpc_bitstream_t *out, int n)
{
	assert(n >= 0);

	while (--n >= 0) {
		if (jpc_bitstream_putbit(out, 1) == EOF) {
			return -1;
		}
	}
	if (jpc_bitstream_putbit(out, 0) == EOF) {
		return -1;
	}
	return 0;
}