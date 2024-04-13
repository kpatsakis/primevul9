cdf_read_header(const cdf_info_t *info, cdf_header_t *h)
{
	char buf[512];

	(void)memcpy(cdf_bo.s, "\01\02\03\04", 4);
	if (cdf_read(info, CAST(off_t, 0), buf, sizeof(buf)) == -1)
		return -1;
	cdf_unpack_header(h, buf);
	cdf_swap_header(h);
	if (h->h_magic != CDF_MAGIC) {
		DPRINTF(("Bad magic %#" INT64_T_FORMAT "x != %#"
		    INT64_T_FORMAT "x\n",
		    (unsigned long long)h->h_magic,
		    (unsigned long long)CDF_MAGIC));
		goto out;
	}
	if (h->h_sec_size_p2 > 20) {
		DPRINTF(("Bad sector size %hu\n", h->h_sec_size_p2));
		goto out;
	}
	if (h->h_short_sec_size_p2 > 20) {
		DPRINTF(("Bad short sector size %hu\n",
		    h->h_short_sec_size_p2));
		goto out;
	}
	return 0;
out:
	errno = EFTYPE;
	return -1;
}