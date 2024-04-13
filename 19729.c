cdf_dump_summary_info(const cdf_header_t *h, const cdf_stream_t *sst)
{
	char buf[128];
	cdf_summary_info_header_t ssi;
	cdf_property_info_t *info;
	size_t count;

	(void)&h;
	if (cdf_unpack_summary_info(sst, h, &ssi, &info, &count) == -1)
		return;
	(void)fprintf(stderr, "Endian: %#x\n", ssi.si_byte_order);
	(void)fprintf(stderr, "Os Version %d.%d\n", ssi.si_os_version & 0xff,
	    ssi.si_os_version >> 8);
	(void)fprintf(stderr, "Os %d\n", ssi.si_os);
	cdf_print_classid(buf, sizeof(buf), &ssi.si_class);
	(void)fprintf(stderr, "Class %s\n", buf);
	(void)fprintf(stderr, "Count %d\n", ssi.si_count);
	cdf_dump_property_info(info, count);
	free(info);
}