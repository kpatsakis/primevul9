cdf_count_chain(const cdf_sat_t *sat, cdf_secid_t sid, size_t size)
{
	size_t i, j;
	cdf_secid_t maxsector = CAST(cdf_secid_t, (sat->sat_len * size)
	    / sizeof(maxsector));

	DPRINTF(("Chain:"));
	if (sid == CDF_SECID_END_OF_CHAIN) {
		/* 0-length chain. */
		DPRINTF((" empty\n"));
		return 0;
	}

	for (j = i = 0; sid >= 0; i++, j++) {
		DPRINTF((" %d", sid));
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Counting chain loop limit"));
			goto out;
		}
		if (sid >= maxsector) {
			DPRINTF(("Sector %d >= %d\n", sid, maxsector));
			goto out;
		}
		sid = CDF_TOLE4(CAST(uint32_t, sat->sat_tab[sid]));
	}
	if (i == 0) {
		DPRINTF((" none, sid: %d\n", sid));
		goto out;

	}
	DPRINTF(("\n"));
	return i;
out:
	errno = EFTYPE;
	return CAST(size_t, -1);
}