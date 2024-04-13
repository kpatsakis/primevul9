int ring_buffer_unlock_commit(struct ring_buffer *buffer,
			      struct ring_buffer_event *event)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	int cpu = raw_smp_processor_id();

	cpu_buffer = buffer->buffers[cpu];

	rb_commit(cpu_buffer, event);

	rb_wakeups(buffer, cpu_buffer);

	trace_recursive_unlock(cpu_buffer);

	preempt_enable_notrace();

	return 0;
}