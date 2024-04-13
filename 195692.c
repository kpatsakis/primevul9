static inline char *readData(const unsigned char *p, unsigned *off, unsigned len, char *ok, unsigned *datalen)
{
    unsigned char *dat, *q;
    unsigned l, newoff, i;
    if (p[*off] != '|') {
	cli_errmsg("Data start marker missing: %c\n", p[*off]);
	*ok = 0;
	return NULL;
    }
    (*off)++;
    l = readNumber(p, off, len, ok);
    if (!l || !ok) {
	*datalen = l;
	return NULL;
    }
    newoff = *off + 2*l;
    if (newoff > len) {
	cli_errmsg("Line ended while reading data\n");
	*ok = 0;
	return 0;
    }
    dat = cli_malloc(l);
    if (!dat) {
	cli_errmsg("Cannot allocate memory for data\n");
	*ok = 0;
	return NULL;
    }
    q = dat;
    for (i=*off;i<newoff;i += 2) {
	const unsigned char v0 = p[i];
	const unsigned char v1 = p[i+1];
	if (UNLIKELY((v0&0xf0) != 0x60 || (v1&0xf0) != 0x60)) {
	    cli_errmsg("Invalid data part: %c%c\n", v0, v1);
	    *ok = 0;
	    return 0;
	}
	*q++ = (v0&0xf) | ((v1&0xf) << 4);
    }
    *off = newoff;
    *datalen = l;
    return (char*)dat;
}