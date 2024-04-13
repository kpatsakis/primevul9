static size_t bin_to_readable(unsigned char *in, size_t inlen, char *out, char nbits) /* {{{ */
{
	unsigned char *p, *q;
	unsigned short w;
	size_t len = inlen;
	int mask;
	int have;

	p = (unsigned char *)in;
	q = (unsigned char *)in + inlen;

	w = 0;
	have = 0;
	mask = (1 << nbits) - 1;

	while (inlen--) {
		if (have < nbits) {
			if (p < q) {
				w |= *p++ << have;
				have += 8;
			} else {
				/* consumed everything? */
				if (have == 0) break;
				/* No? We need a final round */
				have = nbits;
			}
		}

		/* consume nbits */
		*out++ = hexconvtab[w & mask];
		w >>= nbits;
		have -= nbits;
	}

	*out = '\0';
	return len;
}