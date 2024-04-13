dns_zone_setsigresigninginterval(dns_zone_t *zone, uint32_t interval) {
	isc_time_t now;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->sigresigninginterval = interval;
	set_resigntime(zone);
	if (zone->task != NULL) {
		TIME_NOW(&now);
		zone_settimer(zone, &now);
	}
	UNLOCK_ZONE(zone);
}