u64 ring_buffer_time_stamp(struct trace_buffer *buffer)
{
	u64 time;

	preempt_disable_notrace();
	time = rb_time_stamp(buffer);
	preempt_enable_notrace();

	return time;
}