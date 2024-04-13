cdf_timespec_to_timestamp(cdf_timestamp_t *t, const struct timeval *ts)
{
#ifndef __lint__
	(void)&t;
	(void)&ts;
#endif
#ifdef notyet
	struct tm tm;
	if (gmtime_r(&ts->ts_sec, &tm) == NULL) {
		errno = EINVAL;
		return -1;
	}
	*t = (ts->ts_usec / CDF_TIME_PREC) * CDF_TIME_PREC;
	*t = tm.tm_sec;
	*t += tm.tm_min * 60;
	*t += tm.tm_hour * 60 * 60;
	*t += tm.tm_mday * 60 * 60 * 24;
#endif
	return 0;
}