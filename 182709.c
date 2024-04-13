dns_zone_getloadtime(dns_zone_t *zone, isc_time_t *loadtime) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(loadtime != NULL);

	LOCK_ZONE(zone);
	*loadtime = zone->loadtime;
	UNLOCK_ZONE(zone);
	return (ISC_R_SUCCESS);
}