dns_zone_getautomatic(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->automatic);
}