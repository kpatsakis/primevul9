PHP_FUNCTION(gethostbynamel)
{
	char *hostname;
	size_t hostname_len;
	struct hostent *hp;
	struct in_addr in;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &hostname, &hostname_len) == FAILURE) {
		return;
	}

	if(hostname_len > MAXFQDNLEN) {
		/* name too long, protect from CVE-2015-0235 */
		php_error_docref(NULL, E_WARNING, "Host name is too long, the limit is %d characters", MAXFQDNLEN);
		RETURN_FALSE;
	}

	hp = php_network_gethostbyname(hostname);
	if (hp == NULL || hp->h_addr_list == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0 ; hp->h_addr_list[i] != 0 ; i++) {
		in = *(struct in_addr *) hp->h_addr_list[i];
		add_next_index_string(return_value, inet_ntoa(in));
	}
}