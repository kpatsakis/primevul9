dns_zone_setrequeststats(dns_zone_t *zone, isc_stats_t *stats) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->requeststats_on && stats == NULL)
		zone->requeststats_on = false;
	else if (!zone->requeststats_on && stats != NULL) {
		if (zone->requeststats == NULL) {
			isc_stats_attach(stats, &zone->requeststats);
			zone->requeststats_on = true;
		}
	}
	UNLOCK_ZONE(zone);
}