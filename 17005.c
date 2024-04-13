main(int argc, char *argv[])
{
	int i;
	cdf_header_t h;
	cdf_sat_t sat, ssat;
	cdf_stream_t sst, scn;
	cdf_dir_t dir;
	cdf_info_t info;

	if (argc < 2) {
		(void)fprintf(stderr, "Usage: %s <filename>\n", getprogname());
		return -1;
	}

	info.i_buf = NULL;
	info.i_len = 0;
	for (i = 1; i < argc; i++) {
		if ((info.i_fd = open(argv[1], O_RDONLY)) == -1)
			err(1, "Cannot open `%s'", argv[1]);

		if (cdf_read_header(&info, &h) == -1)
			err(1, "Cannot read header");
#ifdef CDF_DEBUG
		cdf_dump_header(&h);
#endif

		if (cdf_read_sat(&info, &h, &sat) == -1)
			err(1, "Cannot read sat");
#ifdef CDF_DEBUG
		cdf_dump_sat("SAT", &sat, CDF_SEC_SIZE(&h));
#endif

		if (cdf_read_ssat(&info, &h, &sat, &ssat) == -1)
			err(1, "Cannot read ssat");
#ifdef CDF_DEBUG
		cdf_dump_sat("SSAT", &ssat, CDF_SHORT_SEC_SIZE(&h));
#endif

		if (cdf_read_dir(&info, &h, &sat, &dir) == -1)
			err(1, "Cannot read dir");

		if (cdf_read_short_stream(&info, &h, &sat, &dir, &sst) == -1)
			err(1, "Cannot read short stream");
#ifdef CDF_DEBUG
		cdf_dump_stream(&h, &sst);
#endif

#ifdef CDF_DEBUG
		cdf_dump_dir(&info, &h, &sat, &ssat, &sst, &dir);
#endif


		if (cdf_read_summary_info(&info, &h, &sat, &ssat, &sst, &dir,
		    &scn) == -1)
			err(1, "Cannot read summary info");
#ifdef CDF_DEBUG
		cdf_dump_summary_info(&h, &scn);
#endif

		(void)close(info.i_fd);
	}

	return 0;
}