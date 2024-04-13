static inline struct page *skb_frag_page(const skb_frag_t *frag)
{
	return frag->page.p;
}