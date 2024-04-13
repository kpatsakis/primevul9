void * __meminit alloc_pages_exact_nid(int nid, size_t size, gfp_t gfp_mask)
{
	unsigned int order = get_order(size);
	struct page *p = alloc_pages_node(nid, gfp_mask, order);
	if (!p)
		return NULL;
	return make_alloc_exact((unsigned long)page_address(p), order, size);
}