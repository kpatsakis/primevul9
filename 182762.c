zone_needdump(dns_zone_t *zone, unsigned int delay) {
	const char me[] = "zone_needdump";
	isc_time_t dumptime;
	isc_time_t now;

	/*
	 * 'zone' locked by caller
	 */

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(LOCKED_ZONE(zone));
	ENTER;

	/*
	 * Do we have a place to dump to and are we loaded?
	 */
	if (zone->masterfile == NULL ||
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) == 0)
		return;

	TIME_NOW(&now);
	/* add some noise */
	DNS_ZONE_JITTER_ADD(&now, delay, &dumptime);

	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDDUMP);
	if (isc_time_isepoch(&zone->dumptime) ||
	    isc_time_compare(&zone->dumptime, &dumptime) > 0)
		zone->dumptime = dumptime;
	if (zone->task != NULL)
		zone_settimer(zone, &now);
}