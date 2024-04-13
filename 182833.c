dns_zone_getmaxxfrout(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->maxxfrout);
}