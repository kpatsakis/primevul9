unsigned ring_buffer_event_length(struct ring_buffer_event *event)
{
	unsigned length;

	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);

	length = rb_event_length(event);
	if (event->type_len > RINGBUF_TYPE_DATA_TYPE_LEN_MAX)
		return length;
	length -= RB_EVNT_HDR_SIZE;
	if (length > RB_MAX_SMALL_DATA + sizeof(event->array[0]))
                length -= sizeof(event->array[0]);
	return length;
}