dns_zone_setstatlevel(dns_zone_t *zone, dns_zonestat_level_t level) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->statlevel = level;
}