PHP_FUNCTION(gethostbyname)
{
	char *hostname;
	size_t hostname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &hostname, &hostname_len) == FAILURE) {
		return;
	}

	if(hostname_len > MAXFQDNLEN) {
		/* name too long, protect from CVE-2015-0235 */
		php_error_docref(NULL, E_WARNING, "Host name is too long, the limit is %d characters", MAXFQDNLEN);
		RETURN_STRINGL(hostname, hostname_len);
	}

	RETURN_STR(php_gethostbyname(hostname));
}