process_ecs(ns_client_t *client, isc_buffer_t *buf, size_t optlen) {
	uint16_t family;
	uint8_t addrlen, addrbytes, scope, *paddr;
	isc_netaddr_t caddr;

	/*
	 * If we have already seen a ECS option skip this ECS option.
	 */
	if ((client->attributes & NS_CLIENTATTR_HAVEECS) != 0) {
		isc_buffer_forward(buf, (unsigned int)optlen);
		return (ISC_R_SUCCESS);
	}

	/*
	 * XXXMUKS: Is there any need to repeat these checks here
	 * (except query's scope length) when they are done in the OPT
	 * RDATA fromwire code?
	 */

	if (optlen < 4U) {
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
			      "EDNS client-subnet option too short");
		return (DNS_R_FORMERR);
	}

	family = isc_buffer_getuint16(buf);
	addrlen = isc_buffer_getuint8(buf);
	scope = isc_buffer_getuint8(buf);
	optlen -= 4;

	if (scope != 0U) {
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
			      "EDNS client-subnet option: invalid scope");
		return (DNS_R_OPTERR);
	}

	memset(&caddr, 0, sizeof(caddr));
	switch (family) {
	case 0:
		/*
		 * XXXMUKS: In queries, if FAMILY is set to 0, SOURCE
		 * PREFIX-LENGTH must be 0 and ADDRESS should not be
		 * present as the address and prefix lengths don't make
		 * sense because the family is unknown.
		 */
		if (addrlen != 0U) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
				      "EDNS client-subnet option: invalid "
				      "address length (%u) for FAMILY=0",
				      addrlen);
			return (DNS_R_OPTERR);
		}
		caddr.family = AF_UNSPEC;
		break;
	case 1:
		if (addrlen > 32U) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
				      "EDNS client-subnet option: invalid "
				      "address length (%u) for IPv4",
				      addrlen);
			return (DNS_R_OPTERR);
		}
		caddr.family = AF_INET;
		break;
	case 2:
		if (addrlen > 128U) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
				      "EDNS client-subnet option: invalid "
				      "address length (%u) for IPv6",
				      addrlen);
			return (DNS_R_OPTERR);
		}
		caddr.family = AF_INET6;
		break;
	default:
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
			      "EDNS client-subnet option: invalid family");
		return (DNS_R_OPTERR);
	}

	addrbytes = (addrlen + 7) / 8;
	if (isc_buffer_remaininglength(buf) < addrbytes) {
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
			      "EDNS client-subnet option: address too short");
		return (DNS_R_OPTERR);
	}

	paddr = (uint8_t *) &caddr.type;
	if (addrbytes != 0U) {
		memmove(paddr, isc_buffer_current(buf), addrbytes);
		isc_buffer_forward(buf, addrbytes);
		optlen -= addrbytes;

		if ((addrlen % 8) != 0) {
			uint8_t bits = ~0U << (8 - (addrlen % 8));
			bits &= paddr[addrbytes - 1];
			if (bits != paddr[addrbytes - 1])
				return (DNS_R_OPTERR);
		}
	}

	memmove(&client->ecs_addr, &caddr, sizeof(caddr));
	client->ecs_addrlen = addrlen;
	client->ecs_scope = 0;
	client->attributes |= NS_CLIENTATTR_HAVEECS;

	isc_buffer_forward(buf, (unsigned int)optlen);
	return (ISC_R_SUCCESS);
}