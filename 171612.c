int ttm_pool_populate(struct ttm_tt *ttm, struct ttm_operation_ctx *ctx)
{
	struct ttm_mem_global *mem_glob = ttm->bdev->glob->mem_glob;
	unsigned i;
	int ret;

	if (ttm->state != tt_unpopulated)
		return 0;

	if (ttm_check_under_lowerlimit(mem_glob, ttm->num_pages, ctx))
		return -ENOMEM;

	ret = ttm_get_pages(ttm->pages, ttm->num_pages, ttm->page_flags,
			    ttm->caching_state);
	if (unlikely(ret != 0)) {
		ttm_pool_unpopulate_helper(ttm, 0);
		return ret;
	}

	for (i = 0; i < ttm->num_pages; ++i) {
		ret = ttm_mem_global_alloc_page(mem_glob, ttm->pages[i],
						PAGE_SIZE, ctx);
		if (unlikely(ret != 0)) {
			ttm_pool_unpopulate_helper(ttm, i);
			return -ENOMEM;
		}
	}

	if (unlikely(ttm->page_flags & TTM_PAGE_FLAG_SWAPPED)) {
		ret = ttm_tt_swapin(ttm);
		if (unlikely(ret != 0)) {
			ttm_pool_unpopulate(ttm);
			return ret;
		}
	}

	ttm->state = tt_unbound;
	return 0;
}