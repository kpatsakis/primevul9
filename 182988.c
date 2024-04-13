dns_zone_setrequestixfr(dns_zone_t *zone, bool flag) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->requestixfr = flag;
}