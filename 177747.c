static time_t phar_zip_d2u_time(char *cdtime, char *cddate) /* {{{ */
{
	int dtime = PHAR_GET_16(cdtime), ddate = PHAR_GET_16(cddate);
	struct tm *tm, tmbuf;
	time_t now;

	now = time(NULL);
	tm = php_localtime_r(&now, &tmbuf);

	tm->tm_year = ((ddate>>9)&127) + 1980 - 1900;
	tm->tm_mon = ((ddate>>5)&15) - 1;
	tm->tm_mday = ddate&31;

	tm->tm_hour = (dtime>>11)&31;
	tm->tm_min = (dtime>>5)&63;
	tm->tm_sec = (dtime<<1)&62;

	return mktime(tm);
}