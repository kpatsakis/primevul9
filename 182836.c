dns_zone_notify(dns_zone_t *zone) {
	isc_time_t now;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);

	TIME_NOW(&now);
	zone_settimer(zone, &now);
	UNLOCK_ZONE(zone);
}