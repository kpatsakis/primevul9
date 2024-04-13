dns_zone_getoptions(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->options);
}