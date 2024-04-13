dns_zone_setstats(dns_zone_t *zone, isc_stats_t *stats) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(zone->stats == NULL);

	LOCK_ZONE(zone);
	zone->stats = NULL;
	isc_stats_attach(stats, &zone->stats);
	UNLOCK_ZONE(zone);
}