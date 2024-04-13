dns_zonemgr_setstartupnotifyrate(dns_zonemgr_t *zmgr, unsigned int value) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	setrl(zmgr->startupnotifyrl, &zmgr->startupnotifyrate, value);
}