   Prints the list of people who've contributed to the PHP project */
PHP_FUNCTION(phpcredits)
{
	long flag = PHP_CREDITS_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flag) == FAILURE) {
		return;
	}

	php_print_credits(flag TSRMLS_CC);
	RETURN_TRUE;