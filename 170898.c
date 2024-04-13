__rb_reserve_next(struct ring_buffer_per_cpu *cpu_buffer,
		  struct rb_event_info *info)
{
	struct ring_buffer_event *event;
	struct buffer_page *tail_page;
	unsigned long tail, write;

	/*
	 * If the time delta since the last event is too big to
	 * hold in the time field of the event, then we append a
	 * TIME EXTEND event ahead of the data event.
	 */
	if (unlikely(info->add_timestamp))
		info->length += RB_LEN_TIME_EXTEND;

	/* Don't let the compiler play games with cpu_buffer->tail_page */
	tail_page = info->tail_page = READ_ONCE(cpu_buffer->tail_page);
	write = local_add_return(info->length, &tail_page->write);

	/* set write to only the index of the write */
	write &= RB_WRITE_MASK;
	tail = write - info->length;

	/*
	 * If this is the first commit on the page, then it has the same
	 * timestamp as the page itself.
	 */
	if (!tail)
		info->delta = 0;

	/* See if we shot pass the end of this buffer page */
	if (unlikely(write > BUF_PAGE_SIZE))
		return rb_move_tail(cpu_buffer, tail, info);

	/* We reserved something on the buffer */

	event = __rb_page_index(tail_page, tail);
	kmemcheck_annotate_bitfield(event, bitfield);
	rb_update_event(cpu_buffer, event, info);

	local_inc(&tail_page->entries);

	/*
	 * If this is the first commit on the page, then update
	 * its timestamp.
	 */
	if (!tail)
		tail_page->page->time_stamp = info->ts;

	/* account for these added bytes */
	local_add(info->length, &cpu_buffer->entries_bytes);

	return event;
}