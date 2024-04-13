static void wake_all_kswapds(unsigned int order, const struct alloc_context *ac)
{
	struct zoneref *z;
	struct zone *zone;
	pg_data_t *last_pgdat = NULL;

	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist,
					ac->high_zoneidx, ac->nodemask) {
		if (last_pgdat != zone->zone_pgdat)
			wakeup_kswapd(zone, order, ac->high_zoneidx);
		last_pgdat = zone->zone_pgdat;
	}
}