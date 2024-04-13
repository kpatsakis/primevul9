static int phar_call_openssl_signverify(int is_sign, php_stream *fp, zend_off_t end, char *key, int key_len, char **signature, int *signature_len) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval retval, zp[3], openssl;
	zend_string *str;

	ZVAL_STRINGL(&openssl, is_sign ? "openssl_sign" : "openssl_verify", is_sign ? sizeof("openssl_sign")-1 : sizeof("openssl_verify")-1);
	ZVAL_STRINGL(&zp[1], *signature, *signature_len);
	ZVAL_STRINGL(&zp[2], key, key_len);
	php_stream_rewind(fp);
	str = php_stream_copy_to_mem(fp, (size_t) end, 0);
	if (str) {
		ZVAL_STR(&zp[0], str);
	} else {
		ZVAL_EMPTY_STRING(&zp[0]);
	}

	if (end != Z_STRLEN(zp[0])) {
		zval_dtor(&zp[0]);
		zval_dtor(&zp[1]);
		zval_dtor(&zp[2]);
		zval_dtor(&openssl);
		return FAILURE;
	}

	if (FAILURE == zend_fcall_info_init(&openssl, 0, &fci, &fcc, NULL, NULL)) {
		zval_dtor(&zp[0]);
		zval_dtor(&zp[1]);
		zval_dtor(&zp[2]);
		zval_dtor(&openssl);
		return FAILURE;
	}

	fci.param_count = 3;
	fci.params = zp;
	Z_ADDREF(zp[0]);
	if (is_sign) {
		ZVAL_NEW_REF(&zp[1], &zp[1]);
	} else {
		Z_ADDREF(zp[1]);
	}
	Z_ADDREF(zp[2]);

	fci.retval = &retval;

	if (FAILURE == zend_call_function(&fci, &fcc)) {
		zval_dtor(&zp[0]);
		zval_dtor(&zp[1]);
		zval_dtor(&zp[2]);
		zval_dtor(&openssl);
		return FAILURE;
	}

	zval_dtor(&openssl);
	Z_DELREF(zp[0]);

	if (is_sign) {
		ZVAL_UNREF(&zp[1]);
	} else {
		Z_DELREF(zp[1]);
	}
	Z_DELREF(zp[2]);

	zval_dtor(&zp[0]);
	zval_dtor(&zp[2]);

	switch (Z_TYPE(retval)) {
		default:
		case IS_LONG:
			zval_dtor(&zp[1]);
			if (1 == Z_LVAL(retval)) {
				return SUCCESS;
			}
			return FAILURE;
		case IS_TRUE:
			*signature = estrndup(Z_STRVAL(zp[1]), Z_STRLEN(zp[1]));
			*signature_len = Z_STRLEN(zp[1]);
			zval_dtor(&zp[1]);
			return SUCCESS;
		case IS_FALSE:
			zval_dtor(&zp[1]);
			return FAILURE;
	}
}