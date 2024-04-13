allowed(isc_netaddr_t *addr, dns_name_t *signer,
	isc_netaddr_t *ecs_addr, uint8_t ecs_addrlen,
	uint8_t *ecs_scope, dns_acl_t *acl)
{
	int match;
	isc_result_t result;

	if (acl == NULL)
		return (true);
	result = dns_acl_match2(addr, signer, ecs_addr, ecs_addrlen, ecs_scope,
				acl, &ns_g_server->aclenv, &match, NULL);
	if (result == ISC_R_SUCCESS && match > 0)
		return (true);
	return (false);
}