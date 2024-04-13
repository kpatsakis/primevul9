unsigned long ring_buffer_overruns(struct ring_buffer *buffer)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long overruns = 0;
	int cpu;

	/* if you care about this being correct, lock the buffer */
	for_each_buffer_cpu(buffer, cpu) {
		cpu_buffer = buffer->buffers[cpu];
		overruns += local_read(&cpu_buffer->overrun);
	}

	return overruns;
}