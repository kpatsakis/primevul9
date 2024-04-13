dns_zone_getrefreshkeytime(dns_zone_t *zone, isc_time_t *refreshkeytime) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(refreshkeytime != NULL);

	LOCK_ZONE(zone);
	*refreshkeytime = zone->refreshkeytime;
	UNLOCK_ZONE(zone);
	return (ISC_R_SUCCESS);
}