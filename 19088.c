void php_snmp_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	zval tmp_member;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	int ret;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);

	ret = zend_hash_find(&php_snmp_properties, Z_STRVAL_P(member), Z_STRLEN_P(member) + 1, (void **) &hnd);

	if (ret == SUCCESS && hnd->write_func) {
		hnd->write_func(obj, value TSRMLS_CC);
		if (! PZVAL_IS_REF(value) && Z_REFCOUNT_P(value) == 0) {
			Z_ADDREF_P(value);
			zval_ptr_dtor(&value);
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
		std_hnd->write_property(object, member, value, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}