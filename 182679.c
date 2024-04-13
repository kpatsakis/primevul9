dns_zone_rekey(dns_zone_t *zone, bool fullsign) {
	isc_time_t now;

	if (zone->type == dns_zone_master && zone->task != NULL) {
		LOCK_ZONE(zone);

		if (fullsign)
			zone->keyopts |= DNS_ZONEKEY_FULLSIGN;

		TIME_NOW(&now);
		zone->refreshkeytime = now;
		zone_settimer(zone, &now);

		UNLOCK_ZONE(zone);
	}
}