ZEND_API int add_index_string(zval *arg, ulong index, const char *str, int duplicate) /* {{{ */
{
	zval *tmp;
	size_t _len = strlen(str);

	if (UNEXPECTED(_len > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}