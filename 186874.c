cdf_timestamp_to_timespec(struct timeval *ts, cdf_timestamp_t t)
{
	struct tm tm;
#ifdef HAVE_STRUCT_TM_TM_ZONE
	static char UTC[] = "UTC";
#endif
	int rdays;

	/* XXX 5.14 at least introdced 100 ns intervals, this is to do */
	/* Time interval, in microseconds */
	ts->tv_usec = (t % CDF_TIME_PREC) * CDF_TIME_PREC;

	t /= CDF_TIME_PREC;
	tm.tm_sec = (int)(t % 60);
	t /= 60;

	tm.tm_min = (int)(t % 60);
	t /= 60;

	tm.tm_hour = (int)(t % 24);
	t /= 24;

	/* XXX: Approx */
	tm.tm_year = (int)(CDF_BASE_YEAR + (t / 365));

	rdays = cdf_getdays(tm.tm_year);
	t -= rdays - 1;
	tm.tm_mday = cdf_getday(tm.tm_year, (int)t);
	tm.tm_mon = cdf_getmonth(tm.tm_year, (int)t);
	tm.tm_wday = 0;
	tm.tm_yday = 0;
	tm.tm_isdst = 0;
#ifdef HAVE_STRUCT_TM_TM_GMTOFF
	tm.tm_gmtoff = 0;
#endif
#ifdef HAVE_STRUCT_TM_TM_ZONE
	tm.tm_zone = UTC;
#endif
	tm.tm_year -= 1900;
	ts->tv_sec = mktime(&tm);
	if (ts->tv_sec == -1) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}