static int ttm_alloc_new_pages(struct list_head *pages, gfp_t gfp_flags,
			       int ttm_flags, enum ttm_caching_state cstate,
			       unsigned count, unsigned order)
{
	struct page **caching_array;
	struct page *p;
	int r = 0;
	unsigned i, j, cpages;
	unsigned npages = 1 << order;
	unsigned max_cpages = min(count << order, (unsigned)NUM_PAGES_TO_ALLOC);

	/* allocate array for page caching change */
	caching_array = kmalloc_array(max_cpages, sizeof(struct page *),
				      GFP_KERNEL);

	if (!caching_array) {
		pr_debug("Unable to allocate table for new pages\n");
		return -ENOMEM;
	}

	for (i = 0, cpages = 0; i < count; ++i) {
		p = alloc_pages(gfp_flags, order);

		if (!p) {
			pr_debug("Unable to get page %u\n", i);

			/* store already allocated pages in the pool after
			 * setting the caching state */
			if (cpages) {
				r = ttm_set_pages_caching(caching_array,
							  cstate, cpages);
				if (r)
					ttm_handle_caching_state_failure(pages,
						ttm_flags, cstate,
						caching_array, cpages);
			}
			r = -ENOMEM;
			goto out;
		}

		list_add(&p->lru, pages);

#ifdef CONFIG_HIGHMEM
		/* gfp flags of highmem page should never be dma32 so we
		 * we should be fine in such case
		 */
		if (PageHighMem(p))
			continue;

#endif
		for (j = 0; j < npages; ++j) {
			caching_array[cpages++] = p++;
			if (cpages == max_cpages) {

				r = ttm_set_pages_caching(caching_array,
						cstate, cpages);
				if (r) {
					ttm_handle_caching_state_failure(pages,
						ttm_flags, cstate,
						caching_array, cpages);
					goto out;
				}
				cpages = 0;
			}
		}
	}

	if (cpages) {
		r = ttm_set_pages_caching(caching_array, cstate, cpages);
		if (r)
			ttm_handle_caching_state_failure(pages,
					ttm_flags, cstate,
					caching_array, cpages);
	}
out:
	kfree(caching_array);

	return r;
}