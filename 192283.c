static inline void __skb_frag_set_page(skb_frag_t *frag, struct page *page)
{
	frag->page.p = page;
}