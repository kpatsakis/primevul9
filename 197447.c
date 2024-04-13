ZEND_API int zend_declare_property_string(zend_class_entry *ce, const char *name, int name_length, const char *value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;
	int len = strlen(value);

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		ZVAL_STRINGL(property, zend_strndup(value, len), len, 0);
	} else {
		ALLOC_ZVAL(property);
		ZVAL_STRINGL(property, value, len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}