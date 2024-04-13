dns_zonemgr_setserialqueryrate(dns_zonemgr_t *zmgr, unsigned int value) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	setrl(zmgr->refreshrl, &zmgr->serialqueryrate, value);
	/* XXXMPA seperate out once we have the code to support this. */
	setrl(zmgr->startuprefreshrl, &zmgr->startupserialqueryrate, value);
}