should_compact_retry(struct alloc_context *ac, unsigned int order, int alloc_flags,
		     enum compact_result compact_result,
		     enum compact_priority *compact_priority,
		     int *compaction_retries)
{
	struct zone *zone;
	struct zoneref *z;

	if (!order || order > PAGE_ALLOC_COSTLY_ORDER)
		return false;

	/*
	 * There are setups with compaction disabled which would prefer to loop
	 * inside the allocator rather than hit the oom killer prematurely.
	 * Let's give them a good hope and keep retrying while the order-0
	 * watermarks are OK.
	 */
	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist, ac->high_zoneidx,
					ac->nodemask) {
		if (zone_watermark_ok(zone, 0, min_wmark_pages(zone),
					ac_classzone_idx(ac), alloc_flags))
			return true;
	}
	return false;
}