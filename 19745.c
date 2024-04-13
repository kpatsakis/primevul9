cdf_read_short_sector(const cdf_stream_t *sst, void *buf, size_t offs,
    size_t len, const cdf_header_t *h, cdf_secid_t id)
{
	size_t ss = CDF_SHORT_SEC_SIZE(h);
	size_t pos;

	if (SIZE_T_MAX / ss < CAST(size_t, id))
		return -1;

	pos = CDF_SHORT_SEC_POS(h, id);
	assert(ss == len);
	if (pos + len > CDF_SEC_SIZE(h) * sst->sst_len) {
		DPRINTF(("Out of bounds read %" SIZE_T_FORMAT "u > %"
		    SIZE_T_FORMAT "u\n",
		    pos + len, CDF_SEC_SIZE(h) * sst->sst_len));
		goto out;
	}
	(void)memcpy(RCAST(char *, buf) + offs,
	    RCAST(const char *, sst->sst_tab) + pos, len);
	return len;
out:
	errno = EFTYPE;
	return -1;
}