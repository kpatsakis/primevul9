timelib_time* timelib_strtotime(char *s, size_t len, struct timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper)
{
	Scanner in;
	int t;
	char *e = s + len - 1;

	memset(&in, 0, sizeof(in));
	in.errors = timelib_malloc(sizeof(struct timelib_error_container));
	in.errors->warning_count = 0;
	in.errors->warning_messages = NULL;
	in.errors->error_count = 0;
	in.errors->error_messages = NULL;

	if (len > 0) {
		while (isspace(*s) && s < e) {
			s++;
		}
		while (isspace(*e) && e > s) {
			e--;
		}
	}
	if (e - s < 0) {
		in.time = timelib_time_ctor();
		add_error(&in, "Empty string");
		if (errors) {
			*errors = in.errors;
		} else {
			timelib_error_container_dtor(in.errors);
		}
		in.time->y = in.time->d = in.time->m = in.time->h = in.time->i = in.time->s = in.time->f = in.time->dst = in.time->z = TIMELIB_UNSET;
		in.time->is_localtime = in.time->zone_type = 0;
		return in.time;
	}
	e++;

	in.str = timelib_malloc((e - s) + YYMAXFILL);
	memset(in.str, 0, (e - s) + YYMAXFILL);
	memcpy(in.str, s, (e - s));
	in.lim = in.str + (e - s) + YYMAXFILL;
	in.cur = in.str;
	in.time = timelib_time_ctor();
	in.time->y = TIMELIB_UNSET;
	in.time->d = TIMELIB_UNSET;
	in.time->m = TIMELIB_UNSET;
	in.time->h = TIMELIB_UNSET;
	in.time->i = TIMELIB_UNSET;
	in.time->s = TIMELIB_UNSET;
	in.time->f = TIMELIB_UNSET;
	in.time->z = TIMELIB_UNSET;
	in.time->dst = TIMELIB_UNSET;
	in.tzdb = tzdb;
	in.time->is_localtime = 0;
	in.time->zone_type = 0;
	in.time->relative.days = TIMELIB_UNSET;

	do {
		t = scan(&in, tz_get_wrapper);
#ifdef DEBUG_PARSER
		printf("%d\n", t);
#endif
	} while(t != EOI);

	/* do funky checking whether the parsed time was valid time */
	if (in.time->have_time && !timelib_valid_time( in.time->h, in.time->i, in.time->s)) {
		add_warning(&in, "The parsed time was invalid");
	}
	/* do funky checking whether the parsed date was valid date */
	if (in.time->have_date && !timelib_valid_date( in.time->y, in.time->m, in.time->d)) {
		add_warning(&in, "The parsed date was invalid");
	}

	timelib_free(in.str);
	if (errors) {
		*errors = in.errors;
	} else {
		timelib_error_container_dtor(in.errors);
	}
	return in.time;
}