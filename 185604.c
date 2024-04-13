rb_iter_peek(struct ring_buffer_iter *iter, u64 *ts)
{
	struct trace_buffer *buffer;
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	int nr_loops = 0;

	if (ts)
		*ts = 0;

	cpu_buffer = iter->cpu_buffer;
	buffer = cpu_buffer->buffer;

	/*
	 * Check if someone performed a consuming read to
	 * the buffer. A consuming read invalidates the iterator
	 * and we need to reset the iterator in this case.
	 */
	if (unlikely(iter->cache_read != cpu_buffer->read ||
		     iter->cache_reader_page != cpu_buffer->reader_page))
		rb_iter_reset(iter);

 again:
	if (ring_buffer_iter_empty(iter))
		return NULL;

	/*
	 * As the writer can mess with what the iterator is trying
	 * to read, just give up if we fail to get an event after
	 * three tries. The iterator is not as reliable when reading
	 * the ring buffer with an active write as the consumer is.
	 * Do not warn if the three failures is reached.
	 */
	if (++nr_loops > 3)
		return NULL;

	if (rb_per_cpu_empty(cpu_buffer))
		return NULL;

	if (iter->head >= rb_page_size(iter->head_page)) {
		rb_inc_iter(iter);
		goto again;
	}

	event = rb_iter_head_event(iter);
	if (!event)
		goto again;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		if (rb_null_event(event)) {
			rb_inc_iter(iter);
			goto again;
		}
		rb_advance_iter(iter);
		return event;

	case RINGBUF_TYPE_TIME_EXTEND:
		/* Internal data, OK to advance */
		rb_advance_iter(iter);
		goto again;

	case RINGBUF_TYPE_TIME_STAMP:
		if (ts) {
			*ts = rb_event_time_stamp(event);
			ring_buffer_normalize_time_stamp(cpu_buffer->buffer,
							 cpu_buffer->cpu, ts);
		}
		/* Internal data, OK to advance */
		rb_advance_iter(iter);
		goto again;

	case RINGBUF_TYPE_DATA:
		if (ts && !(*ts)) {
			*ts = iter->read_stamp + event->time_delta;
			ring_buffer_normalize_time_stamp(buffer,
							 cpu_buffer->cpu, ts);
		}
		return event;

	default:
		RB_WARN_ON(cpu_buffer, 1);
	}

	return NULL;
}