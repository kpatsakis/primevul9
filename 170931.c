void ring_buffer_iter_reset(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;

	if (!iter)
		return;

	cpu_buffer = iter->cpu_buffer;

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	rb_iter_reset(iter);
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);
}