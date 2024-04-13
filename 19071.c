static int php_snmp_has_property(zval *object, zval *member, int has_set_exists, const zend_literal *key TSRMLS_DC)
{
	php_snmp_prop_handler *hnd;
	int ret = 0;

	if (zend_hash_find(&php_snmp_properties, Z_STRVAL_P(member), Z_STRLEN_P(member) + 1, (void **)&hnd) == SUCCESS) {
		switch (has_set_exists) {
			case 2:
				ret = 1;
				break;
			case 0: {
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, key TSRMLS_CC);
				if (value != EG(uninitialized_zval_ptr)) {
					ret = Z_TYPE_P(value) != IS_NULL? 1:0;
					/* refcount is 0 */
					Z_ADDREF_P(value);
					zval_ptr_dtor(&value);
				}
				break;
			}
			default: {
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, key TSRMLS_CC);
				if (value != EG(uninitialized_zval_ptr)) {
					convert_to_boolean(value);
					ret = Z_BVAL_P(value)? 1:0;
					/* refcount is 0 */
					Z_ADDREF_P(value);
					zval_ptr_dtor(&value);
				}
				break;
			}
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
		ret = std_hnd->has_property(object, member, has_set_exists, key TSRMLS_CC);
	}
	return ret;
}