
static int timezone_initialize(php_timezone_obj *tzobj, /*const*/ char *tz TSRMLS_DC)
{
	timelib_time *dummy_t = ecalloc(1, sizeof(timelib_time));
	int           dst, not_found;
	char         *orig_tz = tz;

	dummy_t->z = timelib_parse_zone(&tz, &dst, dummy_t, &not_found, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	if (not_found) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad timezone (%s)", orig_tz);
		efree(dummy_t);
		return FAILURE;
	} else {
		set_timezone_from_timelib_time(tzobj, dummy_t);
		free(dummy_t->tz_abbr);
		efree(dummy_t);
		return SUCCESS;
	}