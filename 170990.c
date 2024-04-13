rb_event_data(struct ring_buffer_event *event)
{
	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);
	BUG_ON(event->type_len > RINGBUF_TYPE_DATA_TYPE_LEN_MAX);
	/* If length is in len field, then array[0] has the data */
	if (event->type_len)
		return (void *)&event->array[0];
	/* Otherwise length is in array[0] and array[1] has the data */
	return (void *)&event->array[1];
}