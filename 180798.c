static struct page *__page_frag_cache_refill(struct page_frag_cache *nc,
					     gfp_t gfp_mask)
{
	struct page *page = NULL;
	gfp_t gfp = gfp_mask;

#if (PAGE_SIZE < PAGE_FRAG_CACHE_MAX_SIZE)
	gfp_mask |= __GFP_COMP | __GFP_NOWARN | __GFP_NORETRY |
		    __GFP_NOMEMALLOC;
	page = alloc_pages_node(NUMA_NO_NODE, gfp_mask,
				PAGE_FRAG_CACHE_MAX_ORDER);
	nc->size = page ? PAGE_FRAG_CACHE_MAX_SIZE : PAGE_SIZE;
#endif
	if (unlikely(!page))
		page = alloc_pages_node(NUMA_NO_NODE, gfp, 0);

	nc->va = page ? page_address(page) : NULL;

	return page;
}