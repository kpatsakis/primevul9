static void rb_free_cpu_buffer(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *head = cpu_buffer->pages;
	struct buffer_page *bpage, *tmp;

	free_buffer_page(cpu_buffer->reader_page);

	rb_head_page_deactivate(cpu_buffer);

	if (head) {
		list_for_each_entry_safe(bpage, tmp, head, list) {
			list_del_init(&bpage->list);
			free_buffer_page(bpage);
		}
		bpage = list_entry(head, struct buffer_page, list);
		free_buffer_page(bpage);
	}

	kfree(cpu_buffer);
}