static void ttm_handle_caching_state_failure(struct list_head *pages,
		int ttm_flags, enum ttm_caching_state cstate,
		struct page **failed_pages, unsigned cpages)
{
	unsigned i;
	/* Failed pages have to be freed */
	for (i = 0; i < cpages; ++i) {
		list_del(&failed_pages[i]->lru);
		__free_page(failed_pages[i]);
	}
}