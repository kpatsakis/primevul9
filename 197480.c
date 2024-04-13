ZEND_API int add_next_index_stringl(zval *arg, const char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	if (UNEXPECTED(length > INT_MAX)) {
		zend_error_noreturn(E_ERROR, "String overflow, max size is %d", INT_MAX);
	}
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}