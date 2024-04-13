rb_try_to_discard(struct ring_buffer_per_cpu *cpu_buffer,
		  struct ring_buffer_event *event)
{
	unsigned long new_index, old_index;
	struct buffer_page *bpage;
	unsigned long index;
	unsigned long addr;
	u64 write_stamp;
	u64 delta;

	new_index = rb_event_index(event);
	old_index = new_index + rb_event_ts_length(event);
	addr = (unsigned long)event;
	addr &= PAGE_MASK;

	bpage = READ_ONCE(cpu_buffer->tail_page);

	delta = rb_time_delta(event);

	if (!rb_time_read(&cpu_buffer->write_stamp, &write_stamp))
		return 0;

	/* Make sure the write stamp is read before testing the location */
	barrier();

	if (bpage->page == (void *)addr && rb_page_write(bpage) == old_index) {
		unsigned long write_mask =
			local_read(&bpage->write) & ~RB_WRITE_MASK;
		unsigned long event_length = rb_event_length(event);

		/* Something came in, can't discard */
		if (!rb_time_cmpxchg(&cpu_buffer->write_stamp,
				       write_stamp, write_stamp - delta))
			return 0;

		/*
		 * It's possible that the event time delta is zero
		 * (has the same time stamp as the previous event)
		 * in which case write_stamp and before_stamp could
		 * be the same. In such a case, force before_stamp
		 * to be different than write_stamp. It doesn't
		 * matter what it is, as long as its different.
		 */
		if (!delta)
			rb_time_set(&cpu_buffer->before_stamp, 0);

		/*
		 * If an event were to come in now, it would see that the
		 * write_stamp and the before_stamp are different, and assume
		 * that this event just added itself before updating
		 * the write stamp. The interrupting event will fix the
		 * write stamp for us, and use the before stamp as its delta.
		 */

		/*
		 * This is on the tail page. It is possible that
		 * a write could come in and move the tail page
		 * and write to the next page. That is fine
		 * because we just shorten what is on this page.
		 */
		old_index += write_mask;
		new_index += write_mask;
		index = local_cmpxchg(&bpage->write, old_index, new_index);
		if (index == old_index) {
			/* update counters */
			local_sub(event_length, &cpu_buffer->entries_bytes);
			return 1;
		}
	}

	/* could not discard */
	return 0;
}