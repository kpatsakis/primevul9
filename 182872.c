dns_zone_getxfrsource6(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (&zone->xfrsource6);
}