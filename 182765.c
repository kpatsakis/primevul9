dns_zone_getmaxrecords(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->maxrecords);
}