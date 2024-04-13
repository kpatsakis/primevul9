rb_update_read_stamp(struct ring_buffer_per_cpu *cpu_buffer,
		     struct ring_buffer_event *event)
{
	u64 delta;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return;

	case RINGBUF_TYPE_TIME_EXTEND:
		delta = rb_event_time_stamp(event);
		cpu_buffer->read_stamp += delta;
		return;

	case RINGBUF_TYPE_TIME_STAMP:
		delta = rb_event_time_stamp(event);
		cpu_buffer->read_stamp = delta;
		return;

	case RINGBUF_TYPE_DATA:
		cpu_buffer->read_stamp += event->time_delta;
		return;

	default:
		RB_WARN_ON(cpu_buffer, 1);
	}
	return;
}