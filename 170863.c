static inline int rb_null_event(struct ring_buffer_event *event)
{
	return event->type_len == RINGBUF_TYPE_PADDING && !event->time_delta;
}