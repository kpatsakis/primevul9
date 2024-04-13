static zval* get_zval_property(zval* object, char* name TSRMLS_DC)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zval member;
		zval *data;
		zend_class_entry *old_scope;

		INIT_PZVAL(&member);
		ZVAL_STRING(&member, name, 0);
		old_scope = EG(scope);
		EG(scope) = Z_OBJCE_P(object);
		data = Z_OBJ_HT_P(object)->read_property(object, &member, BP_VAR_IS, 0 TSRMLS_CC);
		if (data == EG(uninitialized_zval_ptr)) {
			/* Hack for bug #32455 */
			zend_property_info *property_info;

			property_info = zend_get_property_info(Z_OBJCE_P(object), &member, 1 TSRMLS_CC);
			EG(scope) = old_scope;
			if (property_info && zend_hash_quick_exists(Z_OBJPROP_P(object), property_info->name, property_info->name_length+1, property_info->h)) {
				return data;
			}
			return NULL;
		}
		EG(scope) = old_scope;
		return data;
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		zval **data_ptr;

		if (zend_hash_find(Z_ARRVAL_P(object), name, strlen(name)+1, (void**)&data_ptr) == SUCCESS) {
		  return *data_ptr;
		}
	}
  return NULL;
}