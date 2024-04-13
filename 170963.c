rb_reader_unlock(struct ring_buffer_per_cpu *cpu_buffer, bool locked)
{
	if (likely(locked))
		raw_spin_unlock(&cpu_buffer->reader_lock);
	return;
}