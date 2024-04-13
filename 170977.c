ring_buffer_read_events_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	return cpu_buffer->read;
}