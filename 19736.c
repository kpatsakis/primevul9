cdf_read_sector(const cdf_info_t *info, void *buf, size_t offs, size_t len,
    const cdf_header_t *h, cdf_secid_t id)
{
	size_t ss = CDF_SEC_SIZE(h);
	size_t pos;

	if (SIZE_T_MAX / ss < CAST(size_t, id))
		return -1;

	pos = CDF_SEC_POS(h, id);
	assert(ss == len);
	return cdf_read(info, CAST(off_t, pos), RCAST(char *, buf) + offs, len);
}