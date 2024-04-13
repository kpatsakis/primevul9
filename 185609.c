static u64 rb_time_delta(struct ring_buffer_event *event)
{
	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return 0;

	case RINGBUF_TYPE_TIME_EXTEND:
		return rb_event_time_stamp(event);

	case RINGBUF_TYPE_TIME_STAMP:
		return 0;

	case RINGBUF_TYPE_DATA:
		return event->time_delta;
	default:
		return 0;
	}
}