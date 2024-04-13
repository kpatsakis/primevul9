ZEND_API int zend_declare_property_null(zend_class_entry *ce, const char *name, int name_length, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
	} else {
		ALLOC_ZVAL(property);
	}
	INIT_ZVAL(*property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}