void shake_page(struct page *p, int access)
{
	if (!PageSlab(p)) {
		lru_add_drain_all();
		if (PageLRU(p))
			return;
		drain_all_pages(page_zone(p));
		if (PageLRU(p) || is_free_buddy_page(p))
			return;
	}

	/*
	 * Only call shrink_node_slabs here (which would also shrink
	 * other caches) if access is not potentially fatal.
	 */
	if (access)
		drop_slab_node(page_to_nid(p));
}