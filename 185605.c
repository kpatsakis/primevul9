rb_reset_cpu(struct ring_buffer_per_cpu *cpu_buffer)
{
	rb_head_page_deactivate(cpu_buffer);

	cpu_buffer->head_page
		= list_entry(cpu_buffer->pages, struct buffer_page, list);
	local_set(&cpu_buffer->head_page->write, 0);
	local_set(&cpu_buffer->head_page->entries, 0);
	local_set(&cpu_buffer->head_page->page->commit, 0);

	cpu_buffer->head_page->read = 0;

	cpu_buffer->tail_page = cpu_buffer->head_page;
	cpu_buffer->commit_page = cpu_buffer->head_page;

	INIT_LIST_HEAD(&cpu_buffer->reader_page->list);
	INIT_LIST_HEAD(&cpu_buffer->new_pages);
	local_set(&cpu_buffer->reader_page->write, 0);
	local_set(&cpu_buffer->reader_page->entries, 0);
	local_set(&cpu_buffer->reader_page->page->commit, 0);
	cpu_buffer->reader_page->read = 0;

	local_set(&cpu_buffer->entries_bytes, 0);
	local_set(&cpu_buffer->overrun, 0);
	local_set(&cpu_buffer->commit_overrun, 0);
	local_set(&cpu_buffer->dropped_events, 0);
	local_set(&cpu_buffer->entries, 0);
	local_set(&cpu_buffer->committing, 0);
	local_set(&cpu_buffer->commits, 0);
	local_set(&cpu_buffer->pages_touched, 0);
	local_set(&cpu_buffer->pages_read, 0);
	cpu_buffer->last_pages_touch = 0;
	cpu_buffer->shortest_full = 0;
	cpu_buffer->read = 0;
	cpu_buffer->read_bytes = 0;

	rb_time_set(&cpu_buffer->write_stamp, 0);
	rb_time_set(&cpu_buffer->before_stamp, 0);

	memset(cpu_buffer->event_stamp, 0, sizeof(cpu_buffer->event_stamp));

	cpu_buffer->lost_events = 0;
	cpu_buffer->last_overrun = 0;

	rb_head_page_activate(cpu_buffer);
}