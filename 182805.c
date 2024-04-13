dns_zone_setupdatedisabled(dns_zone_t *zone, bool state) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->update_disabled = state;
}