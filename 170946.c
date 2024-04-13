rb_head_page_deactivate(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *hd;

	/* Go through the whole list and clear any pointers found. */
	rb_list_head_clear(cpu_buffer->pages);

	list_for_each(hd, cpu_buffer->pages)
		rb_list_head_clear(hd);
}