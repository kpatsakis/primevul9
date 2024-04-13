cdf_realloc(const char *file __attribute__((__unused__)),
    size_t line __attribute__((__unused__)), void *p, size_t n)
{
	DPRINTF(("%s,%" SIZE_T_FORMAT "u: %s %" SIZE_T_FORMAT "u\n",
	    file, line, __func__, n));
	return realloc(p, n);
}