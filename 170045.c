char *timelib_timezone_id_from_abbr(const char *abbr, timelib_long gmtoffset, int isdst)
{
	const timelib_tz_lookup_table *tp;

	tp = abbr_search(abbr, gmtoffset, isdst);
	if (tp) {
		return (tp->full_tz_name);
	} else {
		return NULL;
	}
}