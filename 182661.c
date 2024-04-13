set_addrkeylist(unsigned int count,
		const isc_sockaddr_t *addrs, isc_sockaddr_t **newaddrsp,
		const isc_dscp_t *dscp, isc_dscp_t **newdscpp,
		dns_name_t **names, dns_name_t ***newnamesp,
		isc_mem_t *mctx)
{
	isc_result_t result;
	isc_sockaddr_t *newaddrs = NULL;
	isc_dscp_t *newdscp = NULL;
	dns_name_t **newnames = NULL;
	unsigned int i;

	REQUIRE(newaddrsp != NULL && *newaddrsp == NULL);
	REQUIRE(newdscpp != NULL && *newdscpp == NULL);
	REQUIRE(newnamesp != NULL && *newnamesp == NULL);

	newaddrs = isc_mem_get(mctx, count * sizeof(*newaddrs));
	if (newaddrs == NULL)
		return (ISC_R_NOMEMORY);
	memmove(newaddrs, addrs, count * sizeof(*newaddrs));

	if (dscp != NULL) {
		newdscp = isc_mem_get(mctx, count * sizeof(*newdscp));
		if (newdscp == NULL) {
			isc_mem_put(mctx, newaddrs, count * sizeof(*newaddrs));
			return (ISC_R_NOMEMORY);
		}
		memmove(newdscp, dscp, count * sizeof(*newdscp));
	} else
		newdscp = NULL;

	if (names != NULL) {
		newnames = isc_mem_get(mctx, count * sizeof(*newnames));
		if (newnames == NULL) {
			if (newdscp != NULL)
				isc_mem_put(mctx, newdscp,
					    count * sizeof(*newdscp));
			isc_mem_put(mctx, newaddrs, count * sizeof(*newaddrs));
			return (ISC_R_NOMEMORY);
		}
		for (i = 0; i < count; i++)
			newnames[i] = NULL;
		for (i = 0; i < count; i++) {
			if (names[i] != NULL) {
				newnames[i] = isc_mem_get(mctx,
							 sizeof(dns_name_t));
				if (newnames[i] == NULL)
					goto allocfail;
				dns_name_init(newnames[i], NULL);
				result = dns_name_dup(names[i], mctx,
						      newnames[i]);
				if (result != ISC_R_SUCCESS) {
				allocfail:
					for (i = 0; i < count; i++)
						if (newnames[i] != NULL)
							dns_name_free(
							       newnames[i],
							       mctx);
					isc_mem_put(mctx, newaddrs,
						    count * sizeof(*newaddrs));
					isc_mem_put(mctx, newdscp,
						    count * sizeof(*newdscp));
					isc_mem_put(mctx, newnames,
						    count * sizeof(*newnames));
					return (ISC_R_NOMEMORY);
				}
			}
		}
	} else
		newnames = NULL;

	*newdscpp = newdscp;
	*newaddrsp = newaddrs;
	*newnamesp = newnames;
	return (ISC_R_SUCCESS);
}