dns_zonemgr_getttransfersin(dns_zonemgr_t *zmgr) {
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	return (zmgr->transfersin);
}