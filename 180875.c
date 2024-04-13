void *alloc_pages_exact(size_t size, gfp_t gfp_mask)
{
	unsigned int order = get_order(size);
	unsigned long addr;

	addr = __get_free_pages(gfp_mask, order);
	return make_alloc_exact(addr, order, size);
}