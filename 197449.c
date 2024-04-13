ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, name, name_length, silent, NULL TSRMLS_CC);
	EG(scope) = old_scope;

	return property?*property:NULL;
}