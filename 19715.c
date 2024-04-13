cdf_check_stream_offset(const cdf_stream_t *sst, const cdf_header_t *h,
    const void *p, size_t tail, int line)
{
	const char *b = RCAST(const char *, sst->sst_tab);
	const char *e = RCAST(const char *, p) + tail;
	size_t ss = cdf_check_stream(sst, h);
	/*LINTED*/(void)&line;
	if (e >= b && CAST(size_t, e - b) <= ss * sst->sst_len)
		return 0;
	DPRINTF(("%d: offset begin %p < end %p || %" SIZE_T_FORMAT "u"
	    " > %" SIZE_T_FORMAT "u [%" SIZE_T_FORMAT "u %"
	    SIZE_T_FORMAT "u]\n", line, b, e, (size_t)(e - b),
	    ss * sst->sst_len, ss, sst->sst_len));
	errno = EFTYPE;
	return -1;
}