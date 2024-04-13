dns_zone_getredirecttype(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(zone->type == dns_zone_redirect);

	return (zone->masters == NULL ? dns_zone_master : dns_zone_slave);
}