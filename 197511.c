ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, uint key_len, char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	if (UNEXPECTED(length > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}