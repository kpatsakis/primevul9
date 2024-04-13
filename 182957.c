dns_zone_getrequestixfr(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->requestixfr);
}