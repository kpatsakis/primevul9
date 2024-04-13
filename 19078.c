static HashTable *php_snmp_get_properties(zval *object TSRMLS_DC)
{
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	HashTable *props;
	zval *val;
	char *key;
	uint key_len;
	HashPosition pos;
	ulong num_key;

	obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);
	props = zend_std_get_properties(object TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(&php_snmp_properties, &pos);

	while (zend_hash_get_current_data_ex(&php_snmp_properties, (void**)&hnd, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(&php_snmp_properties, &key, &key_len, &num_key, 0, &pos);
		if (!hnd->read_func || hnd->read_func(obj, &val TSRMLS_CC) != SUCCESS) {
			val = EG(uninitialized_zval_ptr);
			Z_ADDREF_P(val);
		}
		zend_hash_update(props, key, key_len, (void *)&val, sizeof(zval *), NULL);
		zend_hash_move_forward_ex(&php_snmp_properties, &pos);
	}
	return obj->zo.properties;
}