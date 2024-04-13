gmt_offset(struct tm *tm, time_t t)
{
#if defined (HAVE_TM_GMTOFF)
	return tm->tm_gmtoff;
#else
	struct tm g;
	time_t t2;
	g = *gmtime(&t);
	g.tm_isdst = tm->tm_isdst;
	t2 = mktime(&g);
	return (int)difftime(t, t2);
#endif
}