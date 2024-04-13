generate_time(cms_context *cms, SECItem *encoded, time_t when)
{
	static char timebuf[32];
	SECItem whenitem = {.type = SEC_ASN1_UTC_TIME,
			 .data = (unsigned char *)timebuf,
			 .len = 0
	};
	struct tm *tm;

	tm = gmtime(&when);

	whenitem.len = snprintf(timebuf, 32, "%02d%02d%02d%02d%02d%02dZ",
		tm->tm_year % 100, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	if (whenitem.len == 32)
		cmsreterr(-1, cms, "could not encode timestamp");

	if (SEC_ASN1EncodeItem(cms->arena, encoded, &whenitem,
			SEC_UTCTimeTemplate) == NULL)
		cmsreterr(-1, cms, "could not encode timestamp");
	return 0;
}