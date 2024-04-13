dns_zone_getidlein(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->idlein);
}