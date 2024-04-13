ZEND_API void zend_unset_property(zend_class_entry *scope, zval *object, const char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->unset_property) {
		const char *class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);

		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be unset", name, class_name);
	}
	MAKE_STD_ZVAL(property);
	ZVAL_STRINGL(property, name, name_length, 1);
	Z_OBJ_HT_P(object)->unset_property(object, property, 0 TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
}