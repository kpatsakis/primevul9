dns_zone_getaltxfrsource6dscp(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->altxfrsource6dscp);
}