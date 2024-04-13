static void ttm_pool_update_free_locked(struct ttm_page_pool *pool,
		unsigned freed_pages)
{
	pool->npages -= freed_pages;
	pool->nfrees += freed_pages;
}