dns_zone_getrefreshtime(dns_zone_t *zone, isc_time_t *refreshtime) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(refreshtime != NULL);

	LOCK_ZONE(zone);
	*refreshtime = zone->refreshtime;
	UNLOCK_ZONE(zone);
	return (ISC_R_SUCCESS);
}