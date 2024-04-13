static int ras_putint(jas_stream_t *out, int_fast32_t val)
{
	int_fast32_t x;
	int i;
	int c;

	assert(val >= 0);
	x = val;
	for (i = 0; i < 4; i++) {
		c = (x >> (8 * (3 - i))) & 0xff;
		if (jas_stream_putc(out, c) == EOF) {
			return -1;
		}
	}

	return 0;
}