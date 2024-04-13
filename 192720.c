PHP_FUNCTION(hypot)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &num1, &num2) == FAILURE) {
		return;
	}
#if HAVE_HYPOT
	RETURN_DOUBLE(hypot(num1, num2));
#elif defined(_MSC_VER)
	RETURN_DOUBLE(_hypot(num1, num2));
#else
	RETURN_DOUBLE(sqrt((num1 * num1) + (num2 * num2)));
#endif
}