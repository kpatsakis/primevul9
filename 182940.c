dns_zone_getxfrsource6dscp(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->xfrsource6dscp);
}