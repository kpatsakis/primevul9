bool ring_buffer_empty_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;
	bool dolock;
	int ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return true;

	cpu_buffer = buffer->buffers[cpu];
	local_irq_save(flags);
	dolock = rb_reader_lock(cpu_buffer);
	ret = rb_per_cpu_empty(cpu_buffer);
	rb_reader_unlock(cpu_buffer, dolock);
	local_irq_restore(flags);

	return ret;
}