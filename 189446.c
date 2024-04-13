static int jpc_putnumnewpasses(jpc_bitstream_t *out, int n)
{
	int ret;

	if (n <= 0) {
		return -1;
	} else if (n == 1) {
		ret = jpc_bitstream_putbit(out, 0);
	} else if (n == 2) {
		ret = jpc_bitstream_putbits(out, 2, 2);
	} else if (n <= 5) {
		ret = jpc_bitstream_putbits(out, 4, 0xc | (n - 3));
	} else if (n <= 36) {
		ret = jpc_bitstream_putbits(out, 9, 0x1e0 | (n - 6));
	} else if (n <= 164) {
		ret = jpc_bitstream_putbits(out, 16, 0xff80 | (n - 37));
	} else {
		/* The standard has no provision for encoding a larger value.
		In practice, however, it is highly unlikely that this
		limitation will ever be encountered. */
		return -1;
	}

	return (ret != EOF) ? 0 : (-1);
}