static void *dsalloc_pages(size_t size, gfp_t flags, int cpu)
{
	unsigned int order = get_order(size);
	int node = cpu_to_node(cpu);
	struct page *page;

	page = __alloc_pages_node(node, flags | __GFP_ZERO, order);
	return page ? page_address(page) : NULL;
}