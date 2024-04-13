dns_zone_setjournal(dns_zone_t *zone, const char *myjournal) {
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	result = dns_zone_setstring(zone, &zone->journal, myjournal);
	UNLOCK_ZONE(zone);

	return (result);
}