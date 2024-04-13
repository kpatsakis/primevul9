static inline const char* cli_memmem(const char *haystack, unsigned hlen,
				     const unsigned char *needle, unsigned nlen)
{
    const char *p;
    unsigned char c;
    if (!needle || !haystack) {
	return NULL;
    }
    c = *needle++;
    if (nlen == 1)
	return memchr(haystack, c, hlen);

    while (hlen >= nlen) {
	p = haystack;
	haystack = memchr(haystack, c, hlen - nlen + 1);
	if (!haystack)
	    return NULL;
	hlen -= haystack+1 - p;
	p = haystack + 1;
	if (!memcmp(p, needle, nlen-1))
	    return haystack;
	haystack = p;
    }
    return NULL;
}