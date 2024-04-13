ns_client_addopt(ns_client_t *client, dns_message_t *message,
		 dns_rdataset_t **opt)
{
	unsigned char ecs[ECS_SIZE];
	char nsid[BUFSIZ], *nsidp;
	unsigned char cookie[COOKIE_SIZE];
	isc_result_t result;
	dns_view_t *view;
	dns_resolver_t *resolver;
	uint16_t udpsize;
	dns_ednsopt_t ednsopts[DNS_EDNSOPTIONS];
	int count = 0;
	unsigned int flags;
	unsigned char expire[4];

	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(opt != NULL && *opt == NULL);
	REQUIRE(message != NULL);

	view = client->view;
	resolver = (view != NULL) ? view->resolver : NULL;
	if (resolver != NULL)
		udpsize = dns_resolver_getudpsize(resolver);
	else
		udpsize = ns_g_udpsize;

	flags = client->extflags & DNS_MESSAGEEXTFLAG_REPLYPRESERVE;

	/* Set EDNS options if applicable */
	if (WANTNSID(client) &&
	    (ns_g_server->server_id != NULL ||
	     ns_g_server->server_usehostname)) {
		if (ns_g_server->server_usehostname) {
			result = ns_os_gethostname(nsid, sizeof(nsid));
			if (result != ISC_R_SUCCESS) {
				goto no_nsid;
			}
			nsidp = nsid;
		} else
			nsidp = ns_g_server->server_id;

		INSIST(count < DNS_EDNSOPTIONS);
		ednsopts[count].code = DNS_OPT_NSID;
		ednsopts[count].length = (uint16_t)strlen(nsidp);
		ednsopts[count].value = (unsigned char *)nsidp;
		count++;
	}
 no_nsid:
	if ((client->attributes & NS_CLIENTATTR_WANTCOOKIE) != 0) {
		isc_buffer_t buf;
		isc_stdtime_t now;
		uint32_t nonce;

		isc_buffer_init(&buf, cookie, sizeof(cookie));
		isc_stdtime_get(&now);
		isc_random_get(&nonce);

		compute_cookie(client, now, nonce, ns_g_server->secret, &buf);

		INSIST(count < DNS_EDNSOPTIONS);
		ednsopts[count].code = DNS_OPT_COOKIE;
		ednsopts[count].length = COOKIE_SIZE;
		ednsopts[count].value = cookie;
		count++;
	}
	if ((client->attributes & NS_CLIENTATTR_HAVEEXPIRE) != 0) {
		isc_buffer_t buf;

		INSIST(count < DNS_EDNSOPTIONS);

		isc_buffer_init(&buf, expire, sizeof(expire));
		isc_buffer_putuint32(&buf, client->expire);
		ednsopts[count].code = DNS_OPT_EXPIRE;
		ednsopts[count].length = 4;
		ednsopts[count].value = expire;
		count++;
	}
	if (((client->attributes & NS_CLIENTATTR_HAVEECS) != 0) &&
	    (client->ecs_addr.family == AF_INET ||
	     client->ecs_addr.family == AF_INET6 ||
	     client->ecs_addr.family == AF_UNSPEC))
	{
		isc_buffer_t buf;
		uint8_t addr[16];
		uint32_t plen, addrl;
		uint16_t family = 0;

		/* Add CLIENT-SUBNET option. */

		plen = client->ecs_addrlen;

		/* Round up prefix len to a multiple of 8 */
		addrl = (plen + 7) / 8;

		switch (client->ecs_addr.family) {
		case AF_UNSPEC:
			INSIST(plen == 0);
			family = 0;
			break;
		case AF_INET:
			INSIST(plen <= 32);
			family = 1;
			memmove(addr, &client->ecs_addr.type, addrl);
			break;
		case AF_INET6:
			INSIST(plen <= 128);
			family = 2;
			memmove(addr, &client->ecs_addr.type, addrl);
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}

		isc_buffer_init(&buf, ecs, sizeof(ecs));
		/* family */
		isc_buffer_putuint16(&buf, family);
		/* source prefix-length */
		isc_buffer_putuint8(&buf, client->ecs_addrlen);
		/* scope prefix-length */
		isc_buffer_putuint8(&buf, client->ecs_scope);

		/* address */
		if (addrl > 0) {
			/* Mask off last address byte */
			if ((plen % 8) != 0)
				addr[addrl - 1] &=
					~0U << (8 - (plen % 8));
			isc_buffer_putmem(&buf, addr,
					  (unsigned) addrl);
		}

		ednsopts[count].code = DNS_OPT_CLIENT_SUBNET;
		ednsopts[count].length = addrl + 4;
		ednsopts[count].value = ecs;
		count++;
	}

	result = dns_message_buildopt(message, opt, 0, udpsize, flags,
				      ednsopts, count);
	return (result);
}