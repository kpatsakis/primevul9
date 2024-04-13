cdf_read_long_sector_chain(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, cdf_secid_t sid, size_t len, cdf_stream_t *scn)
{
	size_t ss = CDF_SEC_SIZE(h), i, j;
	ssize_t nr;
	scn->sst_tab = NULL;
	scn->sst_len = cdf_count_chain(sat, sid, ss);
	scn->sst_dirlen = MAX(h->h_min_size_standard_stream, len);
	scn->sst_ss = ss;

	if (sid == CDF_SECID_END_OF_CHAIN || len == 0)
		return cdf_zero_stream(scn);

	if (scn->sst_len == CAST(size_t, -1))
		goto out;

	scn->sst_tab = CDF_CALLOC(scn->sst_len, ss);
	if (scn->sst_tab == NULL)
		return cdf_zero_stream(scn);

	for (j = i = 0; sid >= 0; i++, j++) {
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Read long sector chain loop limit"));
			goto out;
		}
		if (i >= scn->sst_len) {
			DPRINTF(("Out of bounds reading long sector chain "
			    "%" SIZE_T_FORMAT "u > %" SIZE_T_FORMAT "u\n", i,
			    scn->sst_len));
			goto out;
		}
		if ((nr = cdf_read_sector(info, scn->sst_tab, i * ss, ss, h,
		    sid)) != CAST(ssize_t, ss)) {
			if (i == scn->sst_len - 1 && nr > 0) {
				/* Last sector might be truncated */
				return 0;
			}
			DPRINTF(("Reading long sector chain %d", sid));
			goto out;
		}
		sid = CDF_TOLE4(CAST(uint32_t, sat->sat_tab[sid]));
	}
	return 0;
out:
	errno = EFTYPE;
	return cdf_zero_stream(scn);
}