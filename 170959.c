rb_update_iter_read_stamp(struct ring_buffer_iter *iter,
			  struct ring_buffer_event *event)
{
	u64 delta;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return;

	case RINGBUF_TYPE_TIME_EXTEND:
		delta = event->array[0];
		delta <<= TS_SHIFT;
		delta += event->time_delta;
		iter->read_stamp += delta;
		return;

	case RINGBUF_TYPE_TIME_STAMP:
		/* FIXME: not implemented */
		return;

	case RINGBUF_TYPE_DATA:
		iter->read_stamp += event->time_delta;
		return;

	default:
		BUG();
	}
	return;
}