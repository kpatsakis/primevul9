/* {{{ date_period_it_current_key */
static void date_period_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;
	ZVAL_LONG(key, iterator->current_index);