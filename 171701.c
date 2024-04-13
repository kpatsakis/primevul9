bool skb_page_frag_refill(unsigned int sz, struct page_frag *pfrag, gfp_t gfp)
{
	if (pfrag->page) {
		if (page_ref_count(pfrag->page) == 1) {
			pfrag->offset = 0;
			return true;
		}
		if (pfrag->offset + sz <= pfrag->size)
			return true;
		put_page(pfrag->page);
	}

	pfrag->offset = 0;
	if (SKB_FRAG_PAGE_ORDER) {
		/* Avoid direct reclaim but allow kswapd to wake */
		pfrag->page = alloc_pages((gfp & ~__GFP_DIRECT_RECLAIM) |
					  __GFP_COMP | __GFP_NOWARN |
					  __GFP_NORETRY,
					  SKB_FRAG_PAGE_ORDER);
		if (likely(pfrag->page)) {
			pfrag->size = PAGE_SIZE << SKB_FRAG_PAGE_ORDER;
			return true;
		}
	}
	pfrag->page = alloc_page(gfp);
	if (likely(pfrag->page)) {
		pfrag->size = PAGE_SIZE;
		return true;
	}
	return false;
}