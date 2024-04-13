static struct ttm_page_pool *ttm_get_pool(int flags, bool huge,
					  enum ttm_caching_state cstate)
{
	int pool_index;

	if (cstate == tt_cached)
		return NULL;

	if (cstate == tt_wc)
		pool_index = 0x0;
	else
		pool_index = 0x1;

	if (flags & TTM_PAGE_FLAG_DMA32) {
		if (huge)
			return NULL;
		pool_index |= 0x2;

	} else if (huge) {
		pool_index |= 0x4;
	}

	return &_manager->pools[pool_index];
}