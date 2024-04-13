dns_zone_setkeydirectory(dns_zone_t *zone, const char *directory) {
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	result = dns_zone_setstring(zone, &zone->keydirectory, directory);
	UNLOCK_ZONE(zone);

	return (result);
}