cdf_read_short_sector_chain(const cdf_header_t *h,
    const cdf_sat_t *ssat, const cdf_stream_t *sst,
    cdf_secid_t sid, size_t len, cdf_stream_t *scn)
{
	size_t ss = CDF_SHORT_SEC_SIZE(h), i, j;
	scn->sst_tab = NULL;
	scn->sst_len = cdf_count_chain(ssat, sid, CDF_SEC_SIZE(h));
	scn->sst_dirlen = len;
	scn->sst_ss = ss;

	if (scn->sst_len == CAST(size_t, -1))
		goto out;

	scn->sst_tab = CDF_CALLOC(scn->sst_len, ss);
	if (scn->sst_tab == NULL)
		return cdf_zero_stream(scn);

	for (j = i = 0; sid >= 0; i++, j++) {
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Read short sector chain loop limit"));
			goto out;
		}
		if (i >= scn->sst_len) {
			DPRINTF(("Out of bounds reading short sector chain "
			    "%" SIZE_T_FORMAT "u > %" SIZE_T_FORMAT "u\n",
			    i, scn->sst_len));
			goto out;
		}
		if (cdf_read_short_sector(sst, scn->sst_tab, i * ss, ss, h,
		    sid) != CAST(ssize_t, ss)) {
			DPRINTF(("Reading short sector chain %d", sid));
			goto out;
		}
		sid = CDF_TOLE4(CAST(uint32_t, ssat->sat_tab[sid]));
	}
	return 0;
out:
	errno = EFTYPE;
	return cdf_zero_stream(scn);
}