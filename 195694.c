static inline uint64_t readNumber(const unsigned char *p, unsigned *off, unsigned len, char *ok)
{
    uint64_t n=0;
    unsigned i, newoff, lim, p0 = p[*off], shift=0;

    lim = p0 - 0x60;
    if (lim > 0x10) {
	cli_errmsg("Invalid number type: %c\n", p0);
	*ok = 0;
	return 0;
    }
    newoff = *off +lim+1;
    if (newoff > len) {
	cli_errmsg("End of line encountered while reading number\n");
	*ok = 0;
	return 0;
    }

    if (p0 == 0x60) {
	*off = newoff;
	return 0;
    }

    for (i=*off+1;i < newoff; i++) {
	uint64_t v = p[i];
	if (UNLIKELY((v&0xf0) != 0x60)) {
	    cli_errmsg("Invalid number part: %c\n", (char)v);
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