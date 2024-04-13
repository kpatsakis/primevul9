dns_zone_setmaxxfrin(dns_zone_t *zone, uint32_t maxxfrin) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->maxxfrin = maxxfrin;
}