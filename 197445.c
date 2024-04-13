ZEND_API int add_assoc_string_ex(zval *arg, const char *key, uint key_len, char *str, int duplicate) /* {{{ */
{
	zval *tmp;
	size_t _len = strlen(str);

	if (UNEXPECTED(_len > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, _len, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}