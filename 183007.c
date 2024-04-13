dns_zone_getkeydirectory(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->keydirectory);
}