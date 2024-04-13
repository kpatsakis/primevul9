/* {{{ php_strftime - (gm)strftime helper */
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	char                *format, *buf;
	int                  format_len;
	long                 timestamp = 0;
	struct tm            ta;
	int                  max_reallocs = 5;
	size_t               buf_len = 256, real_len;
	timelib_time        *ts;
	timelib_tzinfo      *tzi;
	timelib_time_offset *offset = NULL;

	timestamp = (long) time(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	if (format_len == 0) {
		RETURN_FALSE;
	}

	ts = timelib_time_ctor();
	if (gmt) {
		tzi = NULL;
		timelib_unixtime2gmt(ts, (timelib_sll) timestamp);
	} else {
		tzi = get_timezone_info(TSRMLS_C);
		ts->tz_info = tzi;
		ts->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(ts, (timelib_sll) timestamp);
	}
	ta.tm_sec   = ts->s;
	ta.tm_min   = ts->i;
	ta.tm_hour  = ts->h;
	ta.tm_mday  = ts->d;
	ta.tm_mon   = ts->m - 1;
	ta.tm_year  = ts->y - 1900;
	ta.tm_wday  = timelib_day_of_week(ts->y, ts->m, ts->d);
	ta.tm_yday  = timelib_day_of_year(ts->y, ts->m, ts->d);
	if (gmt) {
		ta.tm_isdst = 0;
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = 0;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = "GMT";
#endif
	} else {
		offset = timelib_get_time_zone_info(timestamp, tzi);

		ta.tm_isdst = offset->is_dst;
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = offset->offset;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = offset->abbr;
#endif
	}

	/* VS2012 crt has a bug where strftime crash with %z and %Z format when the
	   initial buffer is too small. See
	   http://connect.microsoft.com/VisualStudio/feedback/details/759720/vs2012-strftime-crash-with-z-formatting-code */
	buf = (char *) emalloc(buf_len);
	while ((real_len=strftime(buf, buf_len, format, &ta))==buf_len || real_len==0) {
		buf_len *= 2;
		buf = (char *) erealloc(buf, buf_len);
		if (!--max_reallocs) {
			break;
		}
	}
#if defined(PHP_WIN32) && _MSC_VER >= 1700
	/* VS2012 strftime() returns number of characters, not bytes.
		See VC++11 bug id 766205. */
	if (real_len > 0) {
		real_len = strlen(buf);
	}
#endif

	timelib_time_dtor(ts);
	if (!gmt) {
		timelib_time_offset_dtor(offset);
	}

	if (real_len && real_len != buf_len) {
		buf = (char *) erealloc(buf, real_len + 1);
		RETURN_STRINGL(buf, real_len, 0);
	}
	efree(buf);
	RETURN_FALSE;