dns_zone_maintenance(dns_zone_t *zone) {
	const char me[] = "dns_zone_maintenance";
	isc_time_t now;

	REQUIRE(DNS_ZONE_VALID(zone));
	ENTER;

	LOCK_ZONE(zone);
	TIME_NOW(&now);
	zone_settimer(zone, &now);
	UNLOCK_ZONE(zone);
}