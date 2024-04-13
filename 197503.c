ZEND_API int add_property_string_ex(zval *arg, const char *key, uint key_len, const char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;
	size_t _len = strlen(str);

	if (UNEXPECTED(_len > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, _len, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}