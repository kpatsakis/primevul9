dns_zone_setidleout(dns_zone_t *zone, uint32_t idleout) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->idleout = idleout;
}