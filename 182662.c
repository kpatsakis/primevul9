dns_zone_getnotifysrc6(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (&zone->notifysrc6);
}