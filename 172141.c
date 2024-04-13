static void php_filter_array_handler(zval *input, zval **op, zval *return_value, zend_bool add_empty TSRMLS_DC) /* {{{ */
{
	char *arg_key;
	uint arg_key_len;
	ulong index;
	HashPosition pos;
	zval **tmp, **arg_elm;

	if (!op) {
		zval_dtor(return_value);
		MAKE_COPY_ZVAL(&input, return_value);
		php_filter_call(&return_value, FILTER_DEFAULT, NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_LONG) {
		zval_dtor(return_value);
		MAKE_COPY_ZVAL(&input, return_value);
		php_filter_call(&return_value, Z_LVAL_PP(op), NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_ARRAY) {
		array_init(return_value);

		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(op));
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(op), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_PP(op), (void **) &arg_elm, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_PP(op), &pos))
		{
			if (zend_hash_get_current_key_ex(Z_ARRVAL_PP(op), &arg_key, &arg_key_len, &index, 0, &pos) != HASH_KEY_IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Numeric keys are not allowed in the definition array");
				zval_dtor(return_value);
				RETURN_FALSE;
	 		}
			if (arg_key_len < 2) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty keys are not allowed in the definition array");
				zval_dtor(return_value);
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_ARRVAL_P(input), arg_key, arg_key_len, (void **)&tmp) != SUCCESS) {
				if (add_empty) {
					add_assoc_null_ex(return_value, arg_key, arg_key_len);
				}
			} else {
				zval *nval;

				ALLOC_ZVAL(nval);
				MAKE_COPY_ZVAL(tmp, nval);

				php_filter_call(&nval, -1, arg_elm, 0, FILTER_REQUIRE_SCALAR TSRMLS_CC);
				add_assoc_zval_ex(return_value, arg_key, arg_key_len, nval);
			}
		}
	} else {
		RETURN_FALSE;
	}
}