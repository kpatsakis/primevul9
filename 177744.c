static void phar_zip_u2d_time(time_t time, char *dtime, char *ddate) /* {{{ */
{
	php_uint16 ctime, cdate;
	struct tm *tm, tmbuf;

	tm = php_localtime_r(&time, &tmbuf);
	cdate = ((tm->tm_year+1900-1980)<<9) + ((tm->tm_mon+1)<<5) + tm->tm_mday;
	ctime = ((tm->tm_hour)<<11) + ((tm->tm_min)<<5) + ((tm->tm_sec)>>1);
	PHAR_SET_16(dtime, ctime);
	PHAR_SET_16(ddate, cdate);
}