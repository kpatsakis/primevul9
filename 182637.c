dns_zonemgr_resumexfrs(dns_zonemgr_t *zmgr) {

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	zmgr_resume_xfrs(zmgr, true);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);
}