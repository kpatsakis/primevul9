unsigned long ring_buffer_bytes_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	ret = local_read(&cpu_buffer->entries_bytes) - cpu_buffer->read_bytes;

	return ret;
}