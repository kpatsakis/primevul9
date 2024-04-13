static int ttm_pool_mm_shrink_init(struct ttm_pool_manager *manager)
{
	manager->mm_shrink.count_objects = ttm_pool_shrink_count;
	manager->mm_shrink.scan_objects = ttm_pool_shrink_scan;
	manager->mm_shrink.seeks = 1;
	return register_shrinker(&manager->mm_shrink);
}