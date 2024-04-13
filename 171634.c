static void ttm_page_pool_init_locked(struct ttm_page_pool *pool, gfp_t flags,
		char *name, unsigned int order)
{
	spin_lock_init(&pool->lock);
	pool->fill_lock = false;
	INIT_LIST_HEAD(&pool->list);
	pool->npages = pool->nfrees = 0;
	pool->gfp_flags = flags;
	pool->name = name;
	pool->order = order;
}