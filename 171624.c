ttm_pool_unpopulate_helper(struct ttm_tt *ttm, unsigned mem_count_update)
{
	struct ttm_mem_global *mem_glob = ttm->bdev->glob->mem_glob;
	unsigned i;

	if (mem_count_update == 0)
		goto put_pages;

	for (i = 0; i < mem_count_update; ++i) {
		if (!ttm->pages[i])
			continue;

		ttm_mem_global_free_page(mem_glob, ttm->pages[i], PAGE_SIZE);
	}

put_pages:
	ttm_put_pages(ttm->pages, ttm->num_pages, ttm->page_flags,
		      ttm->caching_state);
	ttm->state = tt_unpopulated;
}