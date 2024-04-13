void ring_buffer_reset(struct ring_buffer *buffer)
{
	int cpu;

	for_each_buffer_cpu(buffer, cpu)
		ring_buffer_reset_cpu(buffer, cpu);
}