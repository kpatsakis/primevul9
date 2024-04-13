static inline bool set_page_guard(struct zone *zone, struct page *page,
				unsigned int order, int migratetype)
{
	struct page_ext *page_ext;

	if (!debug_guardpage_enabled())
		return false;

	if (order >= debug_guardpage_minorder())
		return false;

	page_ext = lookup_page_ext(page);
	if (unlikely(!page_ext))
		return false;

	__set_bit(PAGE_EXT_DEBUG_GUARD, &page_ext->flags);

	INIT_LIST_HEAD(&page->lru);
	set_page_private(page, order);
	/* Guard pages are not available for any usage */
	__mod_zone_freepage_state(zone, -(1 << order), migratetype);

	return true;
}