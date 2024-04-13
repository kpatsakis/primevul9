const static timelib_tz_lookup_table* abbr_search(const char *word, timelib_long gmtoffset, int isdst)
{
	int first_found = 0;
	const timelib_tz_lookup_table  *tp, *first_found_elem = NULL;
	const timelib_tz_lookup_table  *fmp;

	if (strcasecmp("utc", word) == 0 || strcasecmp("gmt", word) == 0) {
		return timelib_timezone_utc;
	}

	for (tp = timelib_timezone_lookup; tp->name; tp++) {
		if (strcasecmp(word, tp->name) == 0) {
			if (!first_found) {
				first_found = 1;
				first_found_elem = tp;
				if (gmtoffset == -1) {
					return tp;
				}
			}
			if (tp->gmtoffset == gmtoffset) {
				return tp;
			}
		}
	}
	if (first_found) {
		return first_found_elem;
	}

	/* Still didn't find anything, let's find the zone solely based on
	 * offset/isdst then */
	for (fmp = timelib_timezone_fallbackmap; fmp->name; fmp++) {
		if ((fmp->gmtoffset * 60) == gmtoffset && fmp->type == isdst) {
			return fmp;
		}
	}
	return NULL;
}