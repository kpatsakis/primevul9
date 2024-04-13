u64 ring_buffer_time_stamp(struct ring_buffer *buffer, int cpu)
{
	u64 time;

	preempt_disable_notrace();
	time = rb_time_stamp(buffer);
	preempt_enable_no_resched_notrace();

	return time;
}