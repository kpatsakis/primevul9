dns_zone_getexpiretime(dns_zone_t *zone, isc_time_t *expiretime) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(expiretime != NULL);

	LOCK_ZONE(zone);
	*expiretime = zone->expiretime;
	UNLOCK_ZONE(zone);
	return (ISC_R_SUCCESS);
}