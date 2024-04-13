void timelib_fill_holes(timelib_time *parsed, timelib_time *now, int options)
{
	if (!(options & TIMELIB_OVERRIDE_TIME) && parsed->have_date && !parsed->have_time) {
		parsed->h = 0;
		parsed->i = 0;
		parsed->s = 0;
		parsed->f = 0;
	}
	if (
		parsed->y != TIMELIB_UNSET || parsed->m != TIMELIB_UNSET || parsed->d != TIMELIB_UNSET ||
		parsed->h != TIMELIB_UNSET || parsed->i != TIMELIB_UNSET || parsed->s != TIMELIB_UNSET
	) {
		if (parsed->f == TIMELIB_UNSET) parsed->f = 0;
	} else {
		if (parsed->f == TIMELIB_UNSET) parsed->f = now->f != TIMELIB_UNSET ? now->f : 0;
	}
	if (parsed->y == TIMELIB_UNSET) parsed->y = now->y != TIMELIB_UNSET ? now->y : 0;
	if (parsed->m == TIMELIB_UNSET) parsed->m = now->m != TIMELIB_UNSET ? now->m : 0;
	if (parsed->d == TIMELIB_UNSET) parsed->d = now->d != TIMELIB_UNSET ? now->d : 0;
	if (parsed->h == TIMELIB_UNSET) parsed->h = now->h != TIMELIB_UNSET ? now->h : 0;
	if (parsed->i == TIMELIB_UNSET) parsed->i = now->i != TIMELIB_UNSET ? now->i : 0;
	if (parsed->s == TIMELIB_UNSET) parsed->s = now->s != TIMELIB_UNSET ? now->s : 0;
	if (parsed->z == TIMELIB_UNSET) parsed->z = now->z != TIMELIB_UNSET ? now->z : 0;
	if (parsed->dst == TIMELIB_UNSET) parsed->dst = now->dst != TIMELIB_UNSET ? now->dst : 0;

	if (!parsed->tz_abbr) {
		parsed->tz_abbr = now->tz_abbr ? timelib_strdup(now->tz_abbr) : NULL;
	}
	if (!parsed->tz_info) {
		parsed->tz_info = now->tz_info ? (!(options & TIMELIB_NO_CLONE) ? timelib_tzinfo_clone(now->tz_info) : now->tz_info) : NULL;
	}
	if (parsed->zone_type == 0 && now->zone_type != 0) {
		parsed->zone_type = now->zone_type;
/*		parsed->tz_abbr = now->tz_abbr ? timelib_strdup(now->tz_abbr) : NULL;
		parsed->tz_info = now->tz_info ? timelib_tzinfo_clone(now->tz_info) : NULL;
*/		parsed->is_localtime = 1;
	}
/*	timelib_dump_date(parsed, 2);
	timelib_dump_date(now, 2);
*/
}