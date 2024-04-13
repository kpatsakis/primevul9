rb_update_event(struct ring_buffer_per_cpu *cpu_buffer,
		struct ring_buffer_event *event,
		struct rb_event_info *info)
{
	unsigned length = info->length;
	u64 delta = info->delta;
	unsigned int nest = local_read(&cpu_buffer->committing) - 1;

	if (!WARN_ON_ONCE(nest >= MAX_NEST))
		cpu_buffer->event_stamp[nest] = info->ts;

	/*
	 * If we need to add a timestamp, then we
	 * add it to the start of the reserved space.
	 */
	if (unlikely(info->add_timestamp))
		rb_add_timestamp(cpu_buffer, &event, info, &delta, &length);

	event->time_delta = delta;
	length -= RB_EVNT_HDR_SIZE;
	if (length > RB_MAX_SMALL_DATA || RB_FORCE_8BYTE_ALIGNMENT) {
		event->type_len = 0;
		event->array[0] = length;
	} else
		event->type_len = DIV_ROUND_UP(length, RB_ALIGNMENT);
}