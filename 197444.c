ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval *property, *value;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->read_property) {
		const char *class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);
		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be read", name, class_name);
	}

	MAKE_STD_ZVAL(property);
	ZVAL_STRINGL(property, name, name_length, 1);
	value = Z_OBJ_HT_P(object)->read_property(object, property, silent?BP_VAR_IS:BP_VAR_R, 0 TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
	return value;
}