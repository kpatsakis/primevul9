zone_expire(dns_zone_t *zone) {
	/*
	 * 'zone' locked by caller.
	 */

	REQUIRE(LOCKED_ZONE(zone));

	dns_zone_log(zone, ISC_LOG_WARNING, "expired");

	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_EXPIRED);
	zone->refresh = DNS_ZONE_DEFAULTREFRESH;
	zone->retry = DNS_ZONE_DEFAULTRETRY;
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_HAVETIMERS);
	zone_unload(zone);
}