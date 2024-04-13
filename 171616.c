void ttm_page_alloc_fini(void)
{
	int i;

	pr_info("Finalizing pool allocator\n");
	ttm_pool_mm_shrink_fini(_manager);

	/* OK to use static buffer since global mutex is no longer used. */
	for (i = 0; i < NUM_POOLS; ++i)
		ttm_page_pool_free(&_manager->pools[i], FREE_ALL_PAGES, true);

	kobject_put(&_manager->kobj);
	_manager = NULL;
}