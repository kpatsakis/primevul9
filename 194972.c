PHP_FUNCTION(dns_get_mx)
{
	char *hostname;
	size_t hostname_len;
	zval *mx_list, *weight_list = NULL;
	int count, qdc;
	u_short type, weight;
	u_char ans[MAXPACKET];
	char buf[MAXHOSTNAMELEN];
	HEADER *hp;
	u_char *cp, *end;
	int i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/|z/", &hostname, &hostname_len, &mx_list, &weight_list) == FAILURE) {
		return;
	}

	zval_dtor(mx_list);
	array_init(mx_list);

	if (weight_list) {
		zval_dtor(weight_list);
		array_init(weight_list);
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

	i = php_dns_search(handle, hostname, C_IN, DNS_T_MX, (u_char *)&ans, sizeof(ans));
	if (i < 0) {
		RETURN_FALSE;
	}
	if (i > (int)sizeof(ans)) {
		i = sizeof(ans);
	}
	hp = (HEADER *)&ans;
	cp = (u_char *)&ans + HFIXEDSZ;
	end = (u_char *)&ans +i;
	for (qdc = ntohs((unsigned short)hp->qdcount); qdc--; cp += i + QFIXEDSZ) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
	}
	count = ntohs((unsigned short)hp->ancount);
	while (--count >= 0 && cp < end) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
		cp += i;
		GETSHORT(type, cp);
		cp += INT16SZ + INT32SZ;
		GETSHORT(i, cp);
		if (type != DNS_T_MX) {
			cp += i;
			continue;
		}
		GETSHORT(weight, cp);
		if ((i = dn_expand(ans, end, cp, buf, sizeof(buf)-1)) < 0) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
		cp += i;
		add_next_index_string(mx_list, buf);
		if (weight_list) {
			add_next_index_long(weight_list, weight);
		}
	}
	php_dns_free_handle(handle);
	RETURN_TRUE;
}