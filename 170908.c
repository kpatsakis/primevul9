rb_event_ts_length(struct ring_buffer_event *event)
{
	unsigned len = 0;

	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND) {
		/* time extends include the data event after it */
		len = RB_LEN_TIME_EXTEND;
		event = skip_time_extend(event);
	}
	return len + rb_event_length(event);
}