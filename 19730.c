cdf_read_dir(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, cdf_dir_t *dir)
{
	size_t i, j;
	size_t ss = CDF_SEC_SIZE(h), ns, nd;
	char *buf;
	cdf_secid_t sid = h->h_secid_first_directory;

	ns = cdf_count_chain(sat, sid, ss);
	if (ns == CAST(size_t, -1))
		return -1;

	nd = ss / CDF_DIRECTORY_SIZE;

	dir->dir_len = ns * nd;
	dir->dir_tab = CAST(cdf_directory_t *,
	    CDF_CALLOC(dir->dir_len, sizeof(dir->dir_tab[0])));
	if (dir->dir_tab == NULL)
		return -1;

	if ((buf = CAST(char *, CDF_MALLOC(ss))) == NULL) {
		free(dir->dir_tab);
		return -1;
	}

	for (j = i = 0; i < ns; i++, j++) {
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Read dir loop limit"));
			goto out;
		}
		if (cdf_read_sector(info, buf, 0, ss, h, sid) !=
		    CAST(ssize_t, ss)) {
			DPRINTF(("Reading directory sector %d", sid));
			goto out;
		}
		for (j = 0; j < nd; j++) {
			cdf_unpack_dir(&dir->dir_tab[i * nd + j],
			    &buf[j * CDF_DIRECTORY_SIZE]);
		}
		sid = CDF_TOLE4(CAST(uint32_t, sat->sat_tab[sid]));
	}
	if (NEED_SWAP)
		for (i = 0; i < dir->dir_len; i++)
			cdf_swap_dir(&dir->dir_tab[i]);
	free(buf);
	return 0;
out:
	free(dir->dir_tab);
	free(buf);
	errno = EFTYPE;
	return -1;
}