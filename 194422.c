static int handleAppendResult( int result, smart_str* loc_name TSRMLS_DC)
{
	intl_error_reset( NULL TSRMLS_CC );
	if( result == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "locale_compose: parameter array element is not a string", 0 TSRMLS_CC );
		smart_str_free(loc_name);
		return 0;
	}
	return 1;
}