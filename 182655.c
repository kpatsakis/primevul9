zone_idetach(dns_zone_t **zonep) {
	dns_zone_t *zone;

	/*
	 * 'zone' locked by caller.
	 */
	REQUIRE(zonep != NULL && DNS_ZONE_VALID(*zonep));
	zone = *zonep;
	REQUIRE(LOCKED_ZONE(*zonep));
	*zonep = NULL;

	INSIST(zone->irefs > 0);
	zone->irefs--;
	INSIST(zone->irefs + isc_refcount_current(&zone->erefs) > 0);
}