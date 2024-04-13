/* {{{ date_period_it_current_data */
static void date_period_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = iterator->object;
	timelib_time   *it_time = object->current;
	php_date_obj   *newdateobj;

	/* Create new object */
	MAKE_STD_ZVAL(iterator->current);
	php_date_instantiate(object->start_ce, iterator->current TSRMLS_CC);
	newdateobj = (php_date_obj *) zend_object_store_get_object(iterator->current TSRMLS_CC);
	newdateobj->time = timelib_time_ctor();
	*newdateobj->time = *it_time;
	if (it_time->tz_abbr) {
		newdateobj->time->tz_abbr = strdup(it_time->tz_abbr);
	}
	if (it_time->tz_info) {
		newdateobj->time->tz_info = it_time->tz_info;
	}
	
	*data = &iterator->current;