ttm_pool_shrink_scan(struct shrinker *shrink, struct shrink_control *sc)
{
	static DEFINE_MUTEX(lock);
	static unsigned start_pool;
	unsigned i;
	unsigned pool_offset;
	struct ttm_page_pool *pool;
	int shrink_pages = sc->nr_to_scan;
	unsigned long freed = 0;
	unsigned int nr_free_pool;

	if (!mutex_trylock(&lock))
		return SHRINK_STOP;
	pool_offset = ++start_pool % NUM_POOLS;
	/* select start pool in round robin fashion */
	for (i = 0; i < NUM_POOLS; ++i) {
		unsigned nr_free = shrink_pages;
		unsigned page_nr;

		if (shrink_pages == 0)
			break;

		pool = &_manager->pools[(i + pool_offset)%NUM_POOLS];
		page_nr = (1 << pool->order);
		/* OK to use static buffer since global mutex is held. */
		nr_free_pool = roundup(nr_free, page_nr) >> pool->order;
		shrink_pages = ttm_page_pool_free(pool, nr_free_pool, true);
		freed += (nr_free_pool - shrink_pages) << pool->order;
		if (freed >= sc->nr_to_scan)
			break;
		shrink_pages <<= pool->order;
	}
	mutex_unlock(&lock);
	return freed;
}