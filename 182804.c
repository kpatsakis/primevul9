dns_zone_setmaxrefreshtime(dns_zone_t *zone, uint32_t val) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(val > 0);

	zone->maxrefresh = val;
}