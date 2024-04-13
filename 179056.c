
static int php_date_period_initialize_from_hash(php_period_obj *period_obj, HashTable *myht TSRMLS_DC)
{
	zval **ht_entry;

	/* this function does no rollback on error */

	if (zend_hash_find(myht, "start", sizeof("start"), (void**) &ht_entry) == SUCCESS) {
		if (Z_TYPE_PP(ht_entry) == IS_OBJECT && Z_OBJCE_PP(ht_entry) == date_ce_date) {
			php_date_obj *date_obj;
			date_obj = zend_object_store_get_object(*ht_entry TSRMLS_CC);
			period_obj->start = timelib_time_clone(date_obj->time);
			period_obj->start_ce = Z_OBJCE_PP(ht_entry);
		} else if (Z_TYPE_PP(ht_entry) != IS_NULL) {
			return 0;
		}
	} else {
		return 0;
	}

	if (zend_hash_find(myht, "end", sizeof("end"), (void**) &ht_entry) == SUCCESS) {
		if (Z_TYPE_PP(ht_entry) == IS_OBJECT && Z_OBJCE_PP(ht_entry) == date_ce_date) {
			php_date_obj *date_obj;
			date_obj = zend_object_store_get_object(*ht_entry TSRMLS_CC);
			period_obj->end = timelib_time_clone(date_obj->time);
		} else if (Z_TYPE_PP(ht_entry) != IS_NULL) {
			return 0;
		}
	} else {
		return 0;
	}

	if (zend_hash_find(myht, "current", sizeof("current"), (void**) &ht_entry) == SUCCESS) {
		if (Z_TYPE_PP(ht_entry) == IS_OBJECT && Z_OBJCE_PP(ht_entry) == date_ce_date) {
			php_date_obj *date_obj;
			date_obj = zend_object_store_get_object(*ht_entry TSRMLS_CC);
			period_obj->current = timelib_time_clone(date_obj->time);
		} else if (Z_TYPE_PP(ht_entry) != IS_NULL)  {
			return 0;
		}
	} else {
		return 0;
	}

	if (zend_hash_find(myht, "interval", sizeof("interval"), (void**) &ht_entry) == SUCCESS) {
		if (Z_TYPE_PP(ht_entry) == IS_OBJECT && Z_OBJCE_PP(ht_entry) == date_ce_interval) {
			php_interval_obj *interval_obj;
			interval_obj = zend_object_store_get_object(*ht_entry TSRMLS_CC);
			period_obj->interval = timelib_rel_time_clone(interval_obj->diff);
		} else { /* interval is required */
			return 0;
		}
	} else {
		return 0;
	}

	if (zend_hash_find(myht, "recurrences", sizeof("recurrences"), (void**) &ht_entry) == SUCCESS &&
			Z_TYPE_PP(ht_entry) == IS_LONG && Z_LVAL_PP(ht_entry) >= 0 && Z_LVAL_PP(ht_entry) <= INT_MAX) {
		period_obj->recurrences = Z_LVAL_PP(ht_entry);
	} else {
		return 0;
	}

	if (zend_hash_find(myht, "include_start_date", sizeof("include_start_date"), (void**) &ht_entry) == SUCCESS &&
			Z_TYPE_PP(ht_entry) == IS_BOOL) {
		period_obj->include_start_date = Z_BVAL_PP(ht_entry);
	} else {
		return 0;
	}

	period_obj->initialized = 1;
	
	return 1;