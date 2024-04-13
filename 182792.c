dns_zone_getfile(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->masterfile);
}