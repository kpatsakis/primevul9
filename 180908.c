bool zone_watermark_ok_safe(struct zone *z, unsigned int order,
			unsigned long mark, int classzone_idx)
{
	long free_pages = zone_page_state(z, NR_FREE_PAGES);

	if (z->percpu_drift_mark && free_pages < z->percpu_drift_mark)
		free_pages = zone_page_state_snapshot(z, NR_FREE_PAGES);

	return __zone_watermark_ok(z, order, mark, classzone_idx, 0,
								free_pages);
}