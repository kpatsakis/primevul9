inc_stats(dns_zone_t *zone, isc_statscounter_t counter) {
	if (zone->stats != NULL)
		isc_stats_increment(zone->stats, counter);
}