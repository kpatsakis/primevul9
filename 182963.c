dns_zone_getstatlevel(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->statlevel);
}