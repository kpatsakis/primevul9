ZEND_API int add_index_stringl(zval *arg, ulong index, const char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	if (UNEXPECTED(length > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}