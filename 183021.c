dns_zonemgr_getnotifyrate(dns_zonemgr_t *zmgr) {
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	return (zmgr->notifyrate);
}