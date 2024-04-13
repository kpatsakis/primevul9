rb_event_length(struct ring_buffer_event *event)
{
	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		if (rb_null_event(event))
			/* undefined */
			return -1;
		return  event->array[0] + RB_EVNT_HDR_SIZE;

	case RINGBUF_TYPE_TIME_EXTEND:
		return RB_LEN_TIME_EXTEND;

	case RINGBUF_TYPE_TIME_STAMP:
		return RB_LEN_TIME_STAMP;

	case RINGBUF_TYPE_DATA:
		return rb_event_data_length(event);
	default:
		BUG();
	}
	/* not hit */
	return 0;
}