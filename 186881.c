main(int argc, char *argv[])
{
	struct timeval ts;
	char buf[25];
	static const cdf_timestamp_t tst = 0x01A5E403C2D59C00ULL;
	static const char *ref = "Sat Apr 23 01:30:00 1977";
	char *p, *q;

	cdf_timestamp_to_timespec(&ts, tst);
	p = cdf_ctime(&ts.tv_sec, buf);
	if ((q = strchr(p, '\n')) != NULL)
		*q = '\0';
	if (strcmp(ref, p) != 0)
		errx(1, "Error date %s != %s\n", ref, p);
	return 0;
}