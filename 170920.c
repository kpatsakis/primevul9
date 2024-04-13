rb_event_data_length(struct ring_buffer_event *event)
{
	unsigned length;

	if (event->type_len)
		length = event->type_len * RB_ALIGNMENT;
	else
		length = event->array[0];
	return length + RB_EVNT_HDR_SIZE;
}