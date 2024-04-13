dns_zone_setrequestexpire(dns_zone_t *zone, bool flag) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->requestexpire = flag;
}