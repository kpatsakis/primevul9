PHP_FUNCTION(gethostbyaddr)
{
	char *addr;
	size_t addr_len;
	zend_string *hostname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &addr, &addr_len) == FAILURE) {
		return;
	}

	hostname = php_gethostbyaddr(addr);

	if (hostname == NULL) {
#if HAVE_IPV6 && HAVE_INET_PTON
		php_error_docref(NULL, E_WARNING, "Address is not a valid IPv4 or IPv6 address");
#else
		php_error_docref(NULL, E_WARNING, "Address is not in a.b.c.d form");
#endif
		RETVAL_FALSE;
	} else {
		RETVAL_STR(hostname);
	}
}