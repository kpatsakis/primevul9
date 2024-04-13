static inline void rb_event_discard(struct ring_buffer_event *event)
{
	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);

	/* array[0] holds the actual length for the discarded event */
	event->array[0] = rb_event_data_length(event) - RB_EVNT_HDR_SIZE;
	event->type_len = RINGBUF_TYPE_PADDING;
	/* time delta must be non zero */
	if (!event->time_delta)
		event->time_delta = 1;
}