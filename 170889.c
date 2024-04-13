void ring_buffer_reset_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer = buffer->buffers[cpu];
	unsigned long flags;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return;

	atomic_inc(&buffer->resize_disabled);
	atomic_inc(&cpu_buffer->record_disabled);

	/* Make sure all commits have finished */
	synchronize_sched();

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);

	if (RB_WARN_ON(cpu_buffer, local_read(&cpu_buffer->committing)))
		goto out;

	arch_spin_lock(&cpu_buffer->lock);

	rb_reset_cpu(cpu_buffer);

	arch_spin_unlock(&cpu_buffer->lock);

 out:
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	atomic_dec(&cpu_buffer->record_disabled);
	atomic_dec(&buffer->resize_disabled);
}