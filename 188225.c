
static int php_info_print(const char *str) /* {{{ */
{
	TSRMLS_FETCH();
	return php_output_write(str, strlen(str) TSRMLS_CC);