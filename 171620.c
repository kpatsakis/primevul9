void ttm_pool_unpopulate(struct ttm_tt *ttm)
{
	ttm_pool_unpopulate_helper(ttm, ttm->num_pages);
}