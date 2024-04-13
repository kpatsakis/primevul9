void *ring_buffer_alloc_read_page(struct ring_buffer *buffer, int cpu)
{
	struct buffer_data_page *bpage;
	struct page *page;

	page = alloc_pages_node(cpu_to_node(cpu),
				GFP_KERNEL | __GFP_NORETRY, 0);
	if (!page)
		return NULL;

	bpage = page_address(page);

	rb_init_page(bpage);

	return bpage;
}