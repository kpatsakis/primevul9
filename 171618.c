static int ttm_get_pages(struct page **pages, unsigned npages, int flags,
			 enum ttm_caching_state cstate)
{
	struct ttm_page_pool *pool = ttm_get_pool(flags, false, cstate);
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	struct ttm_page_pool *huge = ttm_get_pool(flags, true, cstate);
#endif
	struct list_head plist;
	struct page *p = NULL;
	unsigned count, first;
	int r;

	/* No pool for cached pages */
	if (pool == NULL) {
		gfp_t gfp_flags = GFP_USER;
		unsigned i;
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
		unsigned j;
#endif

		/* set zero flag for page allocation if required */
		if (flags & TTM_PAGE_FLAG_ZERO_ALLOC)
			gfp_flags |= __GFP_ZERO;

		if (flags & TTM_PAGE_FLAG_NO_RETRY)
			gfp_flags |= __GFP_RETRY_MAYFAIL;

		if (flags & TTM_PAGE_FLAG_DMA32)
			gfp_flags |= GFP_DMA32;
		else
			gfp_flags |= GFP_HIGHUSER;

		i = 0;
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
		if (!(gfp_flags & GFP_DMA32)) {
			while (npages >= HPAGE_PMD_NR) {
				gfp_t huge_flags = gfp_flags;

				huge_flags |= GFP_TRANSHUGE_LIGHT | __GFP_NORETRY |
					__GFP_KSWAPD_RECLAIM;
				huge_flags &= ~__GFP_MOVABLE;
				huge_flags &= ~__GFP_COMP;
				p = alloc_pages(huge_flags, HPAGE_PMD_ORDER);
				if (!p)
					break;

				for (j = 0; j < HPAGE_PMD_NR; ++j)
					pages[i++] = p++;

				npages -= HPAGE_PMD_NR;
			}
		}
#endif

		first = i;
		while (npages) {
			p = alloc_page(gfp_flags);
			if (!p) {
				pr_debug("Unable to allocate page\n");
				return -ENOMEM;
			}

			/* Swap the pages if we detect consecutive order */
			if (i > first && pages[i - 1] == p - 1)
				swap(p, pages[i - 1]);

			pages[i++] = p;
			--npages;
		}
		return 0;
	}

	count = 0;

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	if (huge && npages >= HPAGE_PMD_NR) {
		INIT_LIST_HEAD(&plist);
		ttm_page_pool_get_pages(huge, &plist, flags, cstate,
					npages / HPAGE_PMD_NR,
					HPAGE_PMD_ORDER);

		list_for_each_entry(p, &plist, lru) {
			unsigned j;

			for (j = 0; j < HPAGE_PMD_NR; ++j)
				pages[count++] = &p[j];
		}
	}
#endif

	INIT_LIST_HEAD(&plist);
	r = ttm_page_pool_get_pages(pool, &plist, flags, cstate,
				    npages - count, 0);

	first = count;
	list_for_each_entry(p, &plist, lru) {
		struct page *tmp = p;

		/* Swap the pages if we detect consecutive order */
		if (count > first && pages[count - 1] == tmp - 1)
			swap(tmp, pages[count - 1]);
		pages[count++] = tmp;
	}

	if (r) {
		/* If there is any pages in the list put them back to
		 * the pool.
		 */
		pr_debug("Failed to allocate extra pages for large request\n");
		ttm_put_pages(pages, count, flags, cstate);
		return r;
	}

	return 0;
}