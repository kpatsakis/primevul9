dns_zone_setsigvalidityinterval(dns_zone_t *zone, uint32_t interval) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->sigvalidityinterval = interval;
}