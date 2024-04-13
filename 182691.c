cancel_refresh(dns_zone_t *zone) {
	const char me[] = "cancel_refresh";
	isc_time_t now;

	/*
	 * 'zone' locked by caller.
	 */

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(LOCKED_ZONE(zone));

	ENTER;

	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);
	TIME_NOW(&now);
	zone_settimer(zone, &now);
}