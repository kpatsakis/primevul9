static inline void skb_propagate_pfmemalloc(struct page *page,
					     struct sk_buff *skb)
{
	if (page && page->pfmemalloc)
		skb->pfmemalloc = true;
}