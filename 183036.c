dns_zonemgr_settransfersin(dns_zonemgr_t *zmgr, uint32_t value) {
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	zmgr->transfersin = value;
}