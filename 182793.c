dns_zone_getnotifydelay(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->notifydelay);
}