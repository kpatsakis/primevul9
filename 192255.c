static inline void __skb_fill_page_desc(struct sk_buff *skb, int i,
					struct page *page, int off, int size)
{
	skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

	/*
	 * Propagate page->pfmemalloc to the skb if we can. The problem is
	 * that not all callers have unique ownership of the page. If
	 * pfmemalloc is set, we check the mapping as a mapping implies
	 * page->index is set (index and pfmemalloc share space).
	 * If it's a valid mapping, we cannot use page->pfmemalloc but we
	 * do not lose pfmemalloc information as the pages would not be
	 * allocated using __GFP_MEMALLOC.
	 */
	frag->page.p		  = page;
	frag->page_offset	  = off;
	skb_frag_size_set(frag, size);

	page = compound_head(page);
	if (page->pfmemalloc && !page->mapping)
		skb->pfmemalloc	= true;
}