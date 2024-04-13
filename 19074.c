static int netsnmp_session_init(php_snmp_session **session_p, int version, char *hostname, char *community, int timeout, int retries TSRMLS_DC)
{
	php_snmp_session *session;
	char *pptr, *host_ptr;
	int force_ipv6 = FALSE;
	int n;
	struct sockaddr **psal;
	struct sockaddr **res;

	*session_p = (php_snmp_session *)emalloc(sizeof(php_snmp_session));
	session = *session_p;
	if (session == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed allocating session");
		return (-1);
	}
	memset(session, 0, sizeof(php_snmp_session));

	snmp_sess_init(session);

	session->version = version;
	session->remote_port = SNMP_PORT;

	session->peername = emalloc(MAX_NAME_LEN);
	if (session->peername == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while copying hostname");
		return (-1);
	}
	/* we copy original hostname for further processing */
	strlcpy(session->peername, hostname, MAX_NAME_LEN);
	host_ptr = session->peername;

	/* Reading the hostname and its optional non-default port number */
	if (*host_ptr == '[') { /* IPv6 address */
		force_ipv6 = TRUE;
		host_ptr++;
		if ((pptr = strchr(host_ptr, ']'))) {
			if (pptr[1] == ':') {
				session->remote_port = atoi(pptr + 2);
			}
			*pptr = '\0';
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "malformed IPv6 address, closing square bracket missing");
			return (-1);
		}
	} else { /* IPv4 address */
		if ((pptr = strchr(host_ptr, ':'))) {
			session->remote_port = atoi(pptr + 1);
			*pptr = '\0';
		}
	}

	/* since Net-SNMP library requires 'udp6:' prefix for all IPv6 addresses (in FQDN form too) we need to
	   perform possible name resolution before running any SNMP queries */
	if ((n = php_network_getaddresses(host_ptr, SOCK_DGRAM, &psal, NULL TSRMLS_CC)) == 0) { /* some resolver error */
		/* warnings sent, bailing out */
		return (-1);
	}

	/* we have everything we need in psal, flush peername and fill it properly */
	*(session->peername) = '\0';
	res = psal;
	while (n-- > 0) {
		pptr = session->peername;
#if HAVE_GETADDRINFO && HAVE_IPV6 && HAVE_INET_NTOP
		if (force_ipv6 && (*res)->sa_family != AF_INET6) {
			res++;
			continue;
		}
		if ((*res)->sa_family == AF_INET6) {
			strcpy(session->peername, "udp6:[");
			pptr = session->peername + strlen(session->peername);
			inet_ntop((*res)->sa_family, &(((struct sockaddr_in6*)(*res))->sin6_addr), pptr, MAX_NAME_LEN);
			strcat(pptr, "]");
		} else if ((*res)->sa_family == AF_INET) {
			inet_ntop((*res)->sa_family, &(((struct sockaddr_in*)(*res))->sin_addr), pptr, MAX_NAME_LEN);
		} else {
			res++;
			continue;
		}
#else
		if ((*res)->sa_family != AF_INET) {
			res++;
			continue;
		}
		strcat(pptr, inet_ntoa(((struct sockaddr_in*)(*res))->sin_addr));
#endif
		break;
	}

	if (strlen(session->peername) == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown failure while resolving '%s'", hostname);
		return (-1);
	}
	/* XXX FIXME
		There should be check for non-empty session->peername!
	*/

	/* put back non-standard SNMP port */
	if (session->remote_port != SNMP_PORT) {
		pptr = session->peername + strlen(session->peername);
		sprintf(pptr, ":%d", session->remote_port);
	}

	php_network_freeaddresses(psal);

	if (version == SNMP_VERSION_3) {
		/* Setting the security name. */
		session->securityName = estrdup(community);
		session->securityNameLen = strlen(session->securityName);
	} else {
		session->authenticator = NULL;
		session->community = (u_char *)estrdup(community);
		session->community_len = strlen(community);
	}

	session->retries = retries;
	session->timeout = timeout;
	return (0);
}