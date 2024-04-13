dns_zone_getmaxttl(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->maxttl);
}