ZEND_API int zend_update_static_property_null(zend_class_entry *scope, const char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_NULL(tmp);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}