dns_zonemgr_releasezone(dns_zonemgr_t *zmgr, dns_zone_t *zone) {
	bool free_now = false;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));
	REQUIRE(zone->zmgr == zmgr);

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	LOCK_ZONE(zone);

	ISC_LIST_UNLINK(zmgr->zones, zone, link);
	zone->zmgr = NULL;
	zmgr->refs--;
	if (zmgr->refs == 0)
		free_now = true;

	UNLOCK_ZONE(zone);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);

	if (free_now)
		zonemgr_free(zmgr);
	ENSURE(zone->zmgr == NULL);
}