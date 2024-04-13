dns_zone_getprivatetype(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->privatetype);
}