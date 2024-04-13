dns_zone_first(dns_zonemgr_t *zmgr, dns_zone_t **first) {
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));
	REQUIRE(first != NULL && *first == NULL);

	*first = ISC_LIST_HEAD(zmgr->zones);
	if (*first == NULL)
		return (ISC_R_NOMORE);
	else
		return (ISC_R_SUCCESS);
}