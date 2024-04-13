exit_check(dns_zone_t *zone) {
	REQUIRE(LOCKED_ZONE(zone));

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_SHUTDOWN) && zone->irefs == 0) {
		/*
		 * DNS_ZONEFLG_SHUTDOWN can only be set if erefs == 0.
		 */
		INSIST(isc_refcount_current(&zone->erefs) == 0);
		return (true);
	}
	return (false);
}