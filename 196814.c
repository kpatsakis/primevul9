PHP_FUNCTION(locale_accept_from_http)
{
	UEnumeration *available;
	char *http_accept = NULL;
	int http_accept_len;
	UErrorCode status = 0;
	int len;
	char resultLocale[INTL_MAX_LOCALE_LEN+1];
	UAcceptResult outResult;

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s", &http_accept, &http_accept_len) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_accept_from_http: unable to parse input parameters", 0 TSRMLS_CC );
		RETURN_FALSE;
	}
	if(http_accept_len > ULOC_FULLNAME_CAPACITY) {
		/* check each fragment, if any bigger than capacity, can't do it due to bug #72533 */
		char *start = http_accept;
		char *end;
		size_t len;
		do {
			end = strchr(start, ',');
			len = end ? end-start : http_accept_len-(start-http_accept);
			if(len > ULOC_FULLNAME_CAPACITY) {
				intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"locale_accept_from_http: locale string too long", 0 TSRMLS_CC );
				RETURN_FALSE;
			}
			if(end) {
				start = end+1;
			}
		} while(end != NULL);
	}

	available = ures_openAvailableLocales(NULL, &status);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to retrieve locale list");
	len = uloc_acceptLanguageFromHTTP(resultLocale, INTL_MAX_LOCALE_LEN,
						&outResult, http_accept, available, &status);
	uenum_close(available);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to find acceptable locale");
	if (len < 0 || outResult == ULOC_ACCEPT_FAILED) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(resultLocale, len, 1);
}