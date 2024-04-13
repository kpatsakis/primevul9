file_fmttime(uint64_t v, int flags, char *buf)
{
	char *pp;
	time_t t = (time_t)v;
	struct tm *tm;

	if (flags & FILE_T_WINDOWS) {
		struct timeval ts;
		cdf_timestamp_to_timespec(&ts, t);
		t = ts.tv_sec;
	}

	if (flags & FILE_T_LOCAL) {
		pp = ctime_r(&t, buf);
	} else {
#ifndef HAVE_DAYLIGHT
		private int daylight = 0;
#ifdef HAVE_TM_ISDST
		private time_t now = (time_t)0;

		if (now == (time_t)0) {
			struct tm *tm1;
			(void)time(&now);
			tm1 = localtime(&now);
			if (tm1 == NULL)
				goto out;
			daylight = tm1->tm_isdst;
		}
#endif /* HAVE_TM_ISDST */
#endif /* HAVE_DAYLIGHT */
		if (daylight)
			t += 3600;
		tm = gmtime(&t);
		if (tm == NULL)
			goto out;
		pp = asctime_r(tm, buf);
	}

	if (pp == NULL)
		goto out;
	pp[strcspn(pp, "\n")] = '\0';
	return pp;
out:
	return strcpy(buf, "*Invalid time*");
}