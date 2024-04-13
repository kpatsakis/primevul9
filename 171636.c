int ttm_page_alloc_init(struct ttm_mem_global *glob, unsigned max_pages)
{
	int ret;
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	unsigned order = HPAGE_PMD_ORDER;
#else
	unsigned order = 0;
#endif

	WARN_ON(_manager);

	pr_info("Initializing pool allocator\n");

	_manager = kzalloc(sizeof(*_manager), GFP_KERNEL);
	if (!_manager)
		return -ENOMEM;

	ttm_page_pool_init_locked(&_manager->wc_pool, GFP_HIGHUSER, "wc", 0);

	ttm_page_pool_init_locked(&_manager->uc_pool, GFP_HIGHUSER, "uc", 0);

	ttm_page_pool_init_locked(&_manager->wc_pool_dma32,
				  GFP_USER | GFP_DMA32, "wc dma", 0);

	ttm_page_pool_init_locked(&_manager->uc_pool_dma32,
				  GFP_USER | GFP_DMA32, "uc dma", 0);

	ttm_page_pool_init_locked(&_manager->wc_pool_huge,
				  (GFP_TRANSHUGE_LIGHT | __GFP_NORETRY |
				   __GFP_KSWAPD_RECLAIM) &
				  ~(__GFP_MOVABLE | __GFP_COMP),
				  "wc huge", order);

	ttm_page_pool_init_locked(&_manager->uc_pool_huge,
				  (GFP_TRANSHUGE_LIGHT | __GFP_NORETRY |
				   __GFP_KSWAPD_RECLAIM) &
				  ~(__GFP_MOVABLE | __GFP_COMP)
				  , "uc huge", order);

	_manager->options.max_size = max_pages;
	_manager->options.small = SMALL_ALLOCATION;
	_manager->options.alloc_size = NUM_PAGES_TO_ALLOC;

	ret = kobject_init_and_add(&_manager->kobj, &ttm_pool_kobj_type,
				   &glob->kobj, "pool");
	if (unlikely(ret != 0))
		goto error;

	ret = ttm_pool_mm_shrink_init(_manager);
	if (unlikely(ret != 0))
		goto error;
	return 0;

error:
	kobject_put(&_manager->kobj);
	_manager = NULL;
	return ret;
}