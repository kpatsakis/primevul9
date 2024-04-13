void *alloc_pgtable_page(int node)
{
	struct page *page;
	void *vaddr = NULL;

	page = alloc_pages_node(node, GFP_ATOMIC | __GFP_ZERO, 0);
	if (page)
		vaddr = page_address(page);
	return vaddr;
}