cdf_dump_header(const cdf_header_t *h)
{
	size_t i;

#define DUMP(a, b) (void)fprintf(stderr, "%40.40s = " a "\n", # b, h->h_ ## b)
#define DUMP2(a, b) (void)fprintf(stderr, "%40.40s = " a " (" a ")\n", # b, \
    h->h_ ## b, 1 << h->h_ ## b)
	DUMP("%d", revision);
	DUMP("%d", version);
	DUMP("%#x", byte_order);
	DUMP2("%d", sec_size_p2);
	DUMP2("%d", short_sec_size_p2);
	DUMP("%d", num_sectors_in_sat);
	DUMP("%d", secid_first_directory);
	DUMP("%d", min_size_standard_stream);
	DUMP("%d", secid_first_sector_in_short_sat);
	DUMP("%d", num_sectors_in_short_sat);
	DUMP("%d", secid_first_sector_in_master_sat);
	DUMP("%d", num_sectors_in_master_sat);
	for (i = 0; i < __arraycount(h->h_master_sat); i++) {
		if (h->h_master_sat[i] == CDF_SECID_FREE)
			break;
		(void)fprintf(stderr, "%35.35s[%.3" SIZE_T_FORMAT "u] = %d\n",
		    "master_sat", i, h->h_master_sat[i]);
	}
}