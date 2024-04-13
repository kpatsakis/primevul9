ring_buffer_lock_reserve(struct ring_buffer *buffer, unsigned long length)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	int cpu;

	/* If we are tracing schedule, we don't want to recurse */
	preempt_disable_notrace();

	if (unlikely(atomic_read(&buffer->record_disabled)))
		goto out;

	cpu = raw_smp_processor_id();

	if (unlikely(!cpumask_test_cpu(cpu, buffer->cpumask)))
		goto out;

	cpu_buffer = buffer->buffers[cpu];

	if (unlikely(atomic_read(&cpu_buffer->record_disabled)))
		goto out;

	if (unlikely(length > BUF_MAX_DATA_SIZE))
		goto out;

	if (unlikely(trace_recursive_lock(cpu_buffer)))
		goto out;

	event = rb_reserve_next_event(buffer, cpu_buffer, length);
	if (!event)
		goto out_unlock;

	return event;

 out_unlock:
	trace_recursive_unlock(cpu_buffer);
 out:
	preempt_enable_notrace();
	return NULL;
}