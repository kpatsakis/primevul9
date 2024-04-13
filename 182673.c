dns_zonemgr_getiolimit(dns_zonemgr_t *zmgr) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	return (zmgr->iolimit);
}