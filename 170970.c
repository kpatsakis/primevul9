bool ring_buffer_empty(struct ring_buffer *buffer)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;
	bool dolock;
	int cpu;
	int ret;

	/* yes this is racy, but if you don't like the race, lock the buffer */
	for_each_buffer_cpu(buffer, cpu) {
		cpu_buffer = buffer->buffers[cpu];
		local_irq_save(flags);
		dolock = rb_reader_lock(cpu_buffer);
		ret = rb_per_cpu_empty(cpu_buffer);
		rb_reader_unlock(cpu_buffer, dolock);
		local_irq_restore(flags);

		if (!ret)
			return false;
	}

	return true;
}