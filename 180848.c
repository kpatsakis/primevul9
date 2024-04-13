bool zone_watermark_ok(struct zone *z, unsigned int order, unsigned long mark,
		      int classzone_idx, unsigned int alloc_flags)
{
	return __zone_watermark_ok(z, order, mark, classzone_idx, alloc_flags,
					zone_page_state(z, NR_FREE_PAGES));
}