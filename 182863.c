dns_zone_setmaxrecords(dns_zone_t *zone, uint32_t val) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->maxrecords = val;
}