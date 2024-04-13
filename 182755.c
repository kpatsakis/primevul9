dns_zone_setmaxxfrout(dns_zone_t *zone, uint32_t maxxfrout) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->maxxfrout = maxxfrout;
}