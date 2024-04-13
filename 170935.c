rb_update_write_stamp(struct ring_buffer_per_cpu *cpu_buffer,
		      struct ring_buffer_event *event)
{
	u64 delta;

	/*
	 * The event first in the commit queue updates the
	 * time stamp.
	 */
	if (rb_event_is_commit(cpu_buffer, event)) {
		/*
		 * A commit event that is first on a page
		 * updates the write timestamp with the page stamp
		 */
		if (!rb_event_index(event))
			cpu_buffer->write_stamp =
				cpu_buffer->commit_page->page->time_stamp;
		else if (event->type_len == RINGBUF_TYPE_TIME_EXTEND) {
			delta = event->array[0];
			delta <<= TS_SHIFT;
			delta += event->time_delta;
			cpu_buffer->write_stamp += delta;
		} else
			cpu_buffer->write_stamp += event->time_delta;
	}
}