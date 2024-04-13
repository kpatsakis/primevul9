dns_zonemgr_createzone(dns_zonemgr_t *zmgr, dns_zone_t **zonep) {
	isc_result_t result;
	isc_mem_t *mctx = NULL;
	dns_zone_t *zone = NULL;
	void *item;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));
	REQUIRE(zonep != NULL && *zonep == NULL);

	if (zmgr->mctxpool == NULL)
		return (ISC_R_FAILURE);

	item = isc_pool_get(zmgr->mctxpool);
	if (item == NULL)
		return (ISC_R_FAILURE);

	isc_mem_attach((isc_mem_t *) item, &mctx);
	result = dns_zone_create(&zone, mctx);
	isc_mem_detach(&mctx);

	if (result == ISC_R_SUCCESS)
		*zonep = zone;

	return (result);
}