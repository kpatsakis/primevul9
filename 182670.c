dns_zone_getnotifysrc4(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (&zone->notifysrc4);
}