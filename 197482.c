ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, const char *name, int name_length, const char *value, int value_len, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		ZVAL_STRINGL(property, zend_strndup(value, value_len), value_len, 0);
	} else {
		ALLOC_ZVAL(property);
		ZVAL_STRINGL(property, value, value_len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}