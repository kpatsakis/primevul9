*/
PHP_METHOD(DatePeriod, __construct)
{
	php_period_obj   *dpobj;
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	zval *start, *end = NULL, *interval;
	long  recurrences = 0, options = 0;
	char *isostr = NULL;
	int   isostr_len = 0;
	timelib_time *clone;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOl|l", &start, date_ce_interface, &interval, date_ce_interval, &recurrences, &options) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOO|l", &start, date_ce_interface, &interval, date_ce_interval, &end, date_ce_interface, &options) == FAILURE) {
			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &isostr, &isostr_len, &options) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "This constructor accepts either (DateTimeInterface, DateInterval, int) OR (DateTimeInterface, DateInterval, DateTime) OR (string) as arguments.");
				zend_restore_error_handling(&error_handling TSRMLS_CC);
				return;
			}
		}
	}

	dpobj = zend_object_store_get_object(getThis() TSRMLS_CC);
	dpobj->current = NULL;

	if (isostr) {
		date_period_initialize(&(dpobj->start), &(dpobj->end), &(dpobj->interval), &recurrences, isostr, isostr_len TSRMLS_CC);
		if (dpobj->start == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain a start date.", isostr);
		}
		if (dpobj->interval == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain an interval.", isostr);
		}
		if (dpobj->end == NULL && recurrences == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ISO interval '%s' did not contain an end date or a recurrence count.", isostr);
		}

		if (dpobj->start) {
			timelib_update_ts(dpobj->start, NULL);
		}
		if (dpobj->end) {
			timelib_update_ts(dpobj->end, NULL);
		}
		dpobj->start_ce = date_ce_date;
	} else {
		/* init */
		intobj  = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);

		/* start date */
		dateobj = (php_date_obj *) zend_object_store_get_object(start TSRMLS_CC);
		clone = timelib_time_ctor();
		memcpy(clone, dateobj->time, sizeof(timelib_time));
		if (dateobj->time->tz_abbr) {
			clone->tz_abbr = strdup(dateobj->time->tz_abbr);
		}
		if (dateobj->time->tz_info) {
			clone->tz_info = dateobj->time->tz_info;
		}
		dpobj->start = clone;
		dpobj->start_ce = Z_OBJCE_P(start);

		/* interval */
		dpobj->interval = timelib_rel_time_clone(intobj->diff);

		/* end date */
		if (end) {
			dateobj = (php_date_obj *) zend_object_store_get_object(end TSRMLS_CC);
			clone = timelib_time_clone(dateobj->time);
			dpobj->end = clone;
		}
	}

	/* options */
	dpobj->include_start_date = !(options & PHP_DATE_PERIOD_EXCLUDE_START_DATE);

	/* recurrrences */
	dpobj->recurrences = recurrences + dpobj->include_start_date;

	dpobj->initialized = 1;

	zend_restore_error_handling(&error_handling TSRMLS_CC);