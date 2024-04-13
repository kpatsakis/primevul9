void php_gd_error_method(int type, const char *format, va_list args)
{
	TSRMLS_FETCH();

	switch (type) {
		case GD_DEBUG:
		case GD_INFO:
		case GD_NOTICE:
			type = E_NOTICE;
			break;
		case GD_WARNING:
			type = E_WARNING;
			break;
		default:
			type = E_ERROR;
	}
	php_verror(NULL, "", type, format, args TSRMLS_CC);
}