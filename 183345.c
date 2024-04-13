ptp_unpack_PTPTIME (const char *str) {
	char ptpdate[40];
	char tmp[5];
	size_t  ptpdatelen;
	struct tm tm;

	if (!str)
		return 0;
	ptpdatelen = strlen(str);
	if (ptpdatelen >= sizeof (ptpdate)) {
		/*ptp_debug (params ,"datelen is larger then size of buffer", ptpdatelen, (int)sizeof(ptpdate));*/
		return 0;
	}
	if (ptpdatelen<15) {
		/*ptp_debug (params ,"datelen is less than 15 (%d)", ptpdatelen);*/
		return 0;
	}
	strncpy (ptpdate, str, sizeof(ptpdate));
	ptpdate[sizeof(ptpdate) - 1] = '\0';

	memset(&tm,0,sizeof(tm));
	strncpy (tmp, ptpdate, 4);
	tmp[4] = 0;
	tm.tm_year=atoi (tmp) - 1900;
	strncpy (tmp, ptpdate + 4, 2);
	tmp[2] = 0;
	tm.tm_mon = atoi (tmp) - 1;
	strncpy (tmp, ptpdate + 6, 2);
	tmp[2] = 0;
	tm.tm_mday = atoi (tmp);
	strncpy (tmp, ptpdate + 9, 2);
	tmp[2] = 0;
	tm.tm_hour = atoi (tmp);
	strncpy (tmp, ptpdate + 11, 2);
	tmp[2] = 0;
	tm.tm_min = atoi (tmp);
	strncpy (tmp, ptpdate + 13, 2);
	tmp[2] = 0;
	tm.tm_sec = atoi (tmp);
	tm.tm_isdst = -1;
	return mktime (&tm);
}