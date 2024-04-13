static int rb_allocate_pages(struct ring_buffer_per_cpu *cpu_buffer,
			     unsigned long nr_pages)
{
	LIST_HEAD(pages);

	WARN_ON(!nr_pages);

	if (__rb_allocate_pages(cpu_buffer, nr_pages, &pages))
		return -ENOMEM;

	/*
	 * The ring buffer page list is a circular list that does not
	 * start and end with a list head. All page list items point to
	 * other pages.
	 */
	cpu_buffer->pages = pages.next;
	list_del(&pages);

	cpu_buffer->nr_pages = nr_pages;

	rb_check_pages(cpu_buffer);

	return 0;
}