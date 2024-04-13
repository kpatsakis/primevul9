ZEND_API int zend_parse_parameter(int flags, int arg_num TSRMLS_DC, zval **arg, const char *spec, ...)
{
	va_list va;
	int ret;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;

	va_start(va, spec);
	ret = zend_parse_arg(arg_num, arg, &va, &spec, quiet TSRMLS_CC);
	va_end(va);

	return ret;
}