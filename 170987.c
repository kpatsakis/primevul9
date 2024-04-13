rb_update_event(struct ring_buffer_per_cpu *cpu_buffer,
		struct ring_buffer_event *event,
		struct rb_event_info *info)
{
	unsigned length = info->length;
	u64 delta = info->delta;

	/* Only a commit updates the timestamp */
	if (unlikely(!rb_event_is_commit(cpu_buffer, event)))
		delta = 0;

	/*
	 * If we need to add a timestamp, then we
	 * add it to the start of the resevered space.
	 */
	if (unlikely(info->add_timestamp)) {
		event = rb_add_time_stamp(event, delta);
		length -= RB_LEN_TIME_EXTEND;
		delta = 0;
	}

	event->time_delta = delta;
	length -= RB_EVNT_HDR_SIZE;
	if (length > RB_MAX_SMALL_DATA || RB_FORCE_8BYTE_ALIGNMENT) {
		event->type_len = 0;
		event->array[0] = length;
	} else
		event->type_len = DIV_ROUND_UP(length, RB_ALIGNMENT);
}