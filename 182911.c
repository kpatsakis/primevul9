dns_zonemgr_setiolimit(dns_zonemgr_t *zmgr, uint32_t iolimit) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));
	REQUIRE(iolimit > 0);

	zmgr->iolimit = iolimit;
}