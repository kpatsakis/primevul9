clear_addresskeylist(isc_sockaddr_t **addrsp, isc_dscp_t **dscpsp,
		     dns_name_t ***keynamesp, unsigned int *countp,
		     isc_mem_t *mctx)
{
	unsigned int count;
	isc_sockaddr_t *addrs;
	isc_dscp_t *dscps;
	dns_name_t **keynames;

	REQUIRE(countp != NULL && addrsp != NULL && dscpsp != NULL &&
		keynamesp != NULL);

	count = *countp;
	*countp = 0;
	addrs = *addrsp;
	*addrsp = NULL;
	dscps = *dscpsp;
	*dscpsp = NULL;
	keynames = *keynamesp;
	*keynamesp = NULL;

	if (addrs != NULL)
		isc_mem_put(mctx, addrs, count * sizeof(isc_sockaddr_t));

	if (dscps != NULL)
		isc_mem_put(mctx, dscps, count * sizeof(isc_dscp_t));

	if (keynames != NULL) {
		unsigned int i;
		for (i = 0; i < count; i++) {
			if (keynames[i] != NULL) {
				dns_name_free(keynames[i], mctx);
				isc_mem_put(mctx, keynames[i],
					    sizeof(dns_name_t));
				keynames[i] = NULL;
			}
		}
		isc_mem_put(mctx, keynames, count * sizeof(dns_name_t *));
	}
}