static void set_zval_property(zval* object, char* name, zval* val TSRMLS_DC)
{
	zend_class_entry *old_scope;

	old_scope = EG(scope);
	EG(scope) = Z_OBJCE_P(object);
	Z_DELREF_P(val);
	add_property_zval(object, name, val);
	EG(scope) = old_scope;
}