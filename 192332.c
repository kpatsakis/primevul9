static inline struct page *__skb_alloc_pages(gfp_t gfp_mask,
					      struct sk_buff *skb,
					      unsigned int order)
{
	struct page *page;

	gfp_mask |= __GFP_COLD;

	if (!(gfp_mask & __GFP_NOMEMALLOC))
		gfp_mask |= __GFP_MEMALLOC;

	page = alloc_pages_node(NUMA_NO_NODE, gfp_mask, order);
	if (skb && page && page->pfmemalloc)
		skb->pfmemalloc = true;

	return page;
}