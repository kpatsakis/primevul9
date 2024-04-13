dns_zone_setnodes(dns_zone_t *zone, uint32_t nodes) {
	REQUIRE(DNS_ZONE_VALID(zone));

	if (nodes == 0)
		nodes = 1;
	zone->nodes = nodes;
}