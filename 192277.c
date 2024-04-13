static inline struct page *__skb_alloc_page(gfp_t gfp_mask,
					     struct sk_buff *skb)
{
	return __skb_alloc_pages(gfp_mask, skb, 0);
}