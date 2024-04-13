dns_zone_getaltxfrsource4dscp(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->altxfrsource4dscp);
}