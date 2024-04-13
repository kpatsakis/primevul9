cdf_read_ssat(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, cdf_sat_t *ssat)
{
	size_t i, j;
	size_t ss = CDF_SEC_SIZE(h);
	cdf_secid_t sid = h->h_secid_first_sector_in_short_sat;

	ssat->sat_tab = NULL;
	ssat->sat_len = cdf_count_chain(sat, sid, ss);
	if (ssat->sat_len == CAST(size_t, -1))
		goto out;

	ssat->sat_tab = CAST(cdf_secid_t *, CDF_CALLOC(ssat->sat_len, ss));
	if (ssat->sat_tab == NULL)
		goto out1;

	for (j = i = 0; sid >= 0; i++, j++) {
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Read short sat sector loop limit"));
			goto out;
		}
		if (i >= ssat->sat_len) {
			DPRINTF(("Out of bounds reading short sector chain "
			    "%" SIZE_T_FORMAT "u > %" SIZE_T_FORMAT "u\n", i,
			    ssat->sat_len));
			goto out;
		}
		if (cdf_read_sector(info, ssat->sat_tab, i * ss, ss, h, sid) !=
		    CAST(ssize_t, ss)) {
			DPRINTF(("Reading short sat sector %d", sid));
			goto out1;
		}
		sid = CDF_TOLE4(CAST(uint32_t, sat->sat_tab[sid]));
	}
	return 0;
out:
	errno = EFTYPE;
out1:
	free(ssat->sat_tab);
	return -1;
}