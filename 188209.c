   Output a page of useful information about PHP and the current request */
PHP_FUNCTION(phpinfo)
{
	long flag = PHP_INFO_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flag) == FAILURE) {
		return;
	}

	/* Andale!  Andale!  Yee-Hah! */
	php_output_start_default(TSRMLS_C);
	php_print_info(flag TSRMLS_CC);
	php_output_end(TSRMLS_C);

	RETURN_TRUE;