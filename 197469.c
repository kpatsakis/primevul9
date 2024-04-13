ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, const char *name, int name_length, const char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRING(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}