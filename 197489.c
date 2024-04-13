ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, int name_length, zval *value TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, name, name_length, 0, NULL TSRMLS_CC);
	EG(scope) = old_scope;
	if (!property) {
		return FAILURE;
	} else {
		if (*property != value) {
			if (PZVAL_IS_REF(*property)) {
				zval_dtor(*property);
				Z_TYPE_PP(property) = Z_TYPE_P(value);
				(*property)->value = value->value;
				if (Z_REFCOUNT_P(value) > 0) {
					zval_copy_ctor(*property);
				} else {
					efree(value);
				}
			} else {
				zval *garbage = *property;

				Z_ADDREF_P(value);
				if (PZVAL_IS_REF(value)) {
					SEPARATE_ZVAL(&value);
				}
				*property = value;
				zval_ptr_dtor(&garbage);
			}
		}
		return SUCCESS;
	}
}