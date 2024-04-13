   Return information about the system PHP was built on */
PHP_FUNCTION(php_uname)
{
	char *mode = "a";
	int modelen = sizeof("a")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &mode, &modelen) == FAILURE) {
		return;
	}
	RETURN_STRING(php_get_uname(*mode), 0);