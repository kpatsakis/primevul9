dns_zonemgr_setnotifyrate(dns_zonemgr_t *zmgr, unsigned int value) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	setrl(zmgr->notifyrl, &zmgr->notifyrate, value);
}