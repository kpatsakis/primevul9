static inline unsigned readFixedNumber(const unsigned char *p, unsigned *off,
				       unsigned len, char *ok, unsigned width)
{
    unsigned i, n=0, shift=0;
    unsigned newoff = *off + width;
    if (newoff > len) {
	cli_errmsg("Newline encountered while reading number\n");
	*ok = 0;
	return 0;
    }
    for (i=*off;i<newoff;i++) {
	unsigned v = p[i];
	if (UNLIKELY((v&0xf0) != 0x60)) {
	    cli_errmsg("Invalid number part: %c\n", v);
	    *ok = 0;
	    return 0;
	}
	v &= 0xf;
	v <<= shift;
	n |= v;
	shift += 4;
    }
    *off = newoff;
    return n;
}