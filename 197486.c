ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type TSRMLS_DC) /* {{{ */
{
	return zend_declare_property_ex(ce, name, name_length, property, access_type, NULL, 0 TSRMLS_CC);
}