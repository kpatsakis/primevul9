ttm_pool_shrink_count(struct shrinker *shrink, struct shrink_control *sc)
{
	unsigned i;
	unsigned long count = 0;
	struct ttm_page_pool *pool;

	for (i = 0; i < NUM_POOLS; ++i) {
		pool = &_manager->pools[i];
		count += (pool->npages << pool->order);
	}

	return count;
}