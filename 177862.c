 */
PS_SERIALIZER_DECODE_FUNC(wddx)
{
	zval retval;
	zval *ent;
	zend_string *key;
	zend_ulong idx;
	int ret;

	if (vallen == 0) {
		return SUCCESS;
	}

	ZVAL_UNDEF(&retval);
	if ((ret = php_wddx_deserialize_ex(val, vallen, &retval)) == SUCCESS) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(retval), idx, key, ent) {
			if (key == NULL) {
				key = zend_long_to_str(idx);
			} else {
				zend_string_addref(key);
			}
			if (php_set_session_var(key, ent, NULL)) {
				if (Z_REFCOUNTED_P(ent)) Z_ADDREF_P(ent);
			}
			PS_ADD_VAR(key);
			zend_string_release(key);
		} ZEND_HASH_FOREACH_END();
	}

	zval_ptr_dtor(&retval);

	return ret;