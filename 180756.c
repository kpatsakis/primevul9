void *page_frag_alloc(struct page_frag_cache *nc,
		      unsigned int fragsz, gfp_t gfp_mask)
{
	unsigned int size = PAGE_SIZE;
	struct page *page;
	int offset;

	if (unlikely(!nc->va)) {
refill:
		page = __page_frag_cache_refill(nc, gfp_mask);
		if (!page)
			return NULL;

#if (PAGE_SIZE < PAGE_FRAG_CACHE_MAX_SIZE)
		/* if size can vary use size else just use PAGE_SIZE */
		size = nc->size;
#endif
		/* Even if we own the page, we do not use atomic_set().
		 * This would break get_page_unless_zero() users.
		 */
		page_ref_add(page, size - 1);

		/* reset page count bias and offset to start of new frag */
		nc->pfmemalloc = page_is_pfmemalloc(page);
		nc->pagecnt_bias = size;
		nc->offset = size;
	}

	offset = nc->offset - fragsz;
	if (unlikely(offset < 0)) {
		page = virt_to_page(nc->va);

		if (!page_ref_sub_and_test(page, nc->pagecnt_bias))
			goto refill;

#if (PAGE_SIZE < PAGE_FRAG_CACHE_MAX_SIZE)
		/* if size can vary use size else just use PAGE_SIZE */
		size = nc->size;
#endif
		/* OK, page count is 0, we can safely set it */
		set_page_count(page, size);

		/* reset page count bias and offset to start of new frag */
		nc->pagecnt_bias = size;
		offset = size - fragsz;
	}

	nc->pagecnt_bias--;
	nc->offset = offset;

	return nc->va + offset;
}