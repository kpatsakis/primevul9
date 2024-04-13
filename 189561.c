ns_client_checkaclsilent(ns_client_t *client, isc_netaddr_t *netaddr,
			 dns_acl_t *acl, bool default_allow)
{
	isc_result_t result;
	isc_netaddr_t tmpnetaddr;
	isc_netaddr_t *ecs_addr = NULL;
	uint8_t ecs_addrlen = 0;
	int match;

	if (acl == NULL) {
		if (default_allow)
			goto allow;
		else
			goto deny;
	}

	if (netaddr == NULL) {
		isc_netaddr_fromsockaddr(&tmpnetaddr, &client->peeraddr);
		netaddr = &tmpnetaddr;
	}

	if ((client->attributes & NS_CLIENTATTR_HAVEECS) != 0) {
		ecs_addr = &client->ecs_addr;
		ecs_addrlen = client->ecs_addrlen;
	}

	result = dns_acl_match2(netaddr, client->signer,
				ecs_addr, ecs_addrlen, NULL, acl,
				&ns_g_server->aclenv, &match, NULL);

	if (result != ISC_R_SUCCESS)
		goto deny; /* Internal error, already logged. */

	if (match > 0)
		goto allow;
	goto deny; /* Negative match or no match. */

 allow:
	return (ISC_R_SUCCESS);

 deny:
	return (DNS_R_REFUSED);
}