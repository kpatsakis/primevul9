PHP_FUNCTION(dns_check_record)
{
#ifndef MAXPACKET
#define MAXPACKET  8192 /* max packet size used internally by BIND */
#endif
	u_char ans[MAXPACKET];
	char *hostname, *rectype = NULL;
	size_t hostname_len, rectype_len = 0;
	int type = T_MX, i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &hostname, &hostname_len, &rectype, &rectype_len) == FAILURE) {
		return;
	}

	if (hostname_len == 0) {
		php_error_docref(NULL, E_WARNING, "Host cannot be empty");
		RETURN_FALSE;
	}

	if (rectype) {
		if (!strcasecmp("A",     rectype)) type = T_A;
		else if (!strcasecmp("NS",    rectype)) type = DNS_T_NS;
		else if (!strcasecmp("MX",    rectype)) type = DNS_T_MX;
		else if (!strcasecmp("PTR",   rectype)) type = DNS_T_PTR;
		else if (!strcasecmp("ANY",   rectype)) type = DNS_T_ANY;
		else if (!strcasecmp("SOA",   rectype)) type = DNS_T_SOA;
		else if (!strcasecmp("CAA",   rectype)) type = DNS_T_CAA;
		else if (!strcasecmp("TXT",   rectype)) type = DNS_T_TXT;
		else if (!strcasecmp("CNAME", rectype)) type = DNS_T_CNAME;
		else if (!strcasecmp("AAAA",  rectype)) type = DNS_T_AAAA;
		else if (!strcasecmp("SRV",   rectype)) type = DNS_T_SRV;
		else if (!strcasecmp("NAPTR", rectype)) type = DNS_T_NAPTR;
		else if (!strcasecmp("A6",    rectype)) type = DNS_T_A6;
		else {
			php_error_docref(NULL, E_WARNING, "Type '%s' not supported", rectype);
			RETURN_FALSE;
		}
	}

#if defined(HAVE_DNS_SEARCH)
	handle = dns_open(NULL);
	if (handle == NULL) {
		RETURN_FALSE;
	}
#elif defined(HAVE_RES_NSEARCH)
    memset(&state, 0, sizeof(state));
    if (res_ninit(handle)) {
			RETURN_FALSE;
	}
#else
	res_init();
#endif

	RETVAL_TRUE;
	i = php_dns_search(handle, hostname, C_IN, type, ans, sizeof(ans));

	if (i < 0) {
		RETVAL_FALSE;
	}

	php_dns_free_handle(handle);
}