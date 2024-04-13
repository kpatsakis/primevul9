static void unset_zval_property(zval* object, char* name TSRMLS_DC)
{
	if (Z_TYPE_P(object) == IS_OBJECT) {
		zval member;
		zend_class_entry *old_scope;

		INIT_PZVAL(&member);
		ZVAL_STRING(&member, name, 0);
		old_scope = EG(scope);
		EG(scope) = Z_OBJCE_P(object);
		Z_OBJ_HT_P(object)->unset_property(object, &member, 0 TSRMLS_CC);
		EG(scope) = old_scope;
	} else if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_hash_del(Z_ARRVAL_P(object), name, strlen(name)+1);
	}
}