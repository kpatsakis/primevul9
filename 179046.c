
static int php_date_modify(zval *object, char *modify, int modify_len TSRMLS_DC)
{
	php_date_obj *dateobj;
	timelib_time *tmp_time;
	timelib_error_container *err = NULL;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	if (!(dateobj->time)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The DateTime object has not been correctly initialized by its constructor");
		return 0;
	}

	tmp_time = timelib_strtotime(modify, modify_len, &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);

	/* update last errors and warnings */
	update_errors_warnings(err TSRMLS_CC);
	if (err && err->error_count) {
		/* spit out the first library error message, at least */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to parse time string (%s) at position %d (%c): %s", modify,
			err->error_messages[0].position, err->error_messages[0].character, err->error_messages[0].message);
		timelib_time_dtor(tmp_time);
		return 0;
	}

	memcpy(&dateobj->time->relative, &tmp_time->relative, sizeof(struct timelib_rel_time));
	dateobj->time->have_relative = tmp_time->have_relative;
	dateobj->time->sse_uptodate = 0;

	if (tmp_time->y != -99999) {
		dateobj->time->y = tmp_time->y;
	}
	if (tmp_time->m != -99999) {
		dateobj->time->m = tmp_time->m;
	}
	if (tmp_time->d != -99999) {
		dateobj->time->d = tmp_time->d;
	}

	if (tmp_time->h != -99999) {
		dateobj->time->h = tmp_time->h;
		if (tmp_time->i != -99999) {
			dateobj->time->i = tmp_time->i;
			if (tmp_time->s != -99999) {
				dateobj->time->s = tmp_time->s;
			} else {
				dateobj->time->s = 0;
			}
		} else {
			dateobj->time->i = 0;
			dateobj->time->s = 0;
		}
	}
	timelib_time_dtor(tmp_time);

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);
	dateobj->time->have_relative = 0;
	
	return 1;