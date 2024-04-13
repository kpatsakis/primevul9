file_regcomp(file_regex_t *rx, const char *pat, int flags)
{
#ifdef USE_C_LOCALE
	rx->c_lc_ctype = newlocale(LC_CTYPE_MASK, "C", 0);
	assert(rx->c_lc_ctype != NULL);
	rx->old_lc_ctype = uselocale(rx->c_lc_ctype);
	assert(rx->old_lc_ctype != NULL);
#endif
	rx->pat = pat;

	return rx->rc = regcomp(&rx->rx, pat, flags);
}