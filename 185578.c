static struct page *new_page(struct page *p, unsigned long private, int **x)
{
	int nid = page_to_nid(p);
	if (PageHuge(p))
		return alloc_huge_page_node(page_hstate(compound_head(p)),
						   nid);
	else
		return __alloc_pages_node(nid, GFP_HIGHUSER_MOVABLE, 0);
}