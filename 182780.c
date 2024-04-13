dns_zone_getmaxxfrin(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->maxxfrin);
}