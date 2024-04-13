rb_update_iter_read_stamp(struct ring_buffer_iter *iter,
			  struct ring_buffer_event *event)
{
	u64 delta;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return;

	case RINGBUF_TYPE_TIME_EXTEND:
		delta = rb_event_time_stamp(event);
		iter->read_stamp += delta;
		return;

	case RINGBUF_TYPE_TIME_STAMP:
		delta = rb_event_time_stamp(event);
		iter->read_stamp = delta;
		return;

	case RINGBUF_TYPE_DATA:
		iter->read_stamp += event->time_delta;
		return;

	default:
		RB_WARN_ON(iter->cpu_buffer, 1);
	}
	return;
}