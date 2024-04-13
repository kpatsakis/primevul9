static void __init mdesc_memblock_free(struct mdesc_handle *hp)
{
	unsigned int alloc_size;
	unsigned long start;

	BUG_ON(refcount_read(&hp->refcnt) != 0);
	BUG_ON(!list_empty(&hp->list));

	alloc_size = PAGE_ALIGN(hp->handle_size);
	start = __pa(hp);
	memblock_free_late(start, alloc_size);
}