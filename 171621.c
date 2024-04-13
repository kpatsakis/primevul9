static void ttm_pool_mm_shrink_fini(struct ttm_pool_manager *manager)
{
	unregister_shrinker(&manager->mm_shrink);
}