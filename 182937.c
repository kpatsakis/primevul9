dns_zone_setrcvquerystats(dns_zone_t *zone, dns_stats_t *stats) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->requeststats_on && stats != NULL) {
		if (zone->rcvquerystats == NULL) {
			dns_stats_attach(stats, &zone->rcvquerystats);
			zone->requeststats_on = true;
		}
	}
	UNLOCK_ZONE(zone);
}