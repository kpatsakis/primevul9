static inline void clear_page_guard(struct zone *zone, struct page *page,
				unsigned int order, int migratetype)
{
	struct page_ext *page_ext;

	if (!debug_guardpage_enabled())
		return;

	page_ext = lookup_page_ext(page);
	if (unlikely(!page_ext))
		return;

	__clear_bit(PAGE_EXT_DEBUG_GUARD, &page_ext->flags);

	set_page_private(page, 0);
	if (!is_migrate_isolate(migratetype))
		__mod_zone_freepage_state(zone, (1 << order), migratetype);
}