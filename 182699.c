dns_zone_setmaxretrytime(dns_zone_t *zone, uint32_t val) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(val > 0);

	zone->maxretry = val;
}