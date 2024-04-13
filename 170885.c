int ring_buffer_swap_cpu(struct ring_buffer *buffer_a,
			 struct ring_buffer *buffer_b, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer_a;
	struct ring_buffer_per_cpu *cpu_buffer_b;
	int ret = -EINVAL;

	if (!cpumask_test_cpu(cpu, buffer_a->cpumask) ||
	    !cpumask_test_cpu(cpu, buffer_b->cpumask))
		goto out;

	cpu_buffer_a = buffer_a->buffers[cpu];
	cpu_buffer_b = buffer_b->buffers[cpu];

	/* At least make sure the two buffers are somewhat the same */
	if (cpu_buffer_a->nr_pages != cpu_buffer_b->nr_pages)
		goto out;

	ret = -EAGAIN;

	if (atomic_read(&buffer_a->record_disabled))
		goto out;

	if (atomic_read(&buffer_b->record_disabled))
		goto out;

	if (atomic_read(&cpu_buffer_a->record_disabled))
		goto out;

	if (atomic_read(&cpu_buffer_b->record_disabled))
		goto out;

	/*
	 * We can't do a synchronize_sched here because this
	 * function can be called in atomic context.
	 * Normally this will be called from the same CPU as cpu.
	 * If not it's up to the caller to protect this.
	 */
	atomic_inc(&cpu_buffer_a->record_disabled);
	atomic_inc(&cpu_buffer_b->record_disabled);

	ret = -EBUSY;
	if (local_read(&cpu_buffer_a->committing))
		goto out_dec;
	if (local_read(&cpu_buffer_b->committing))
		goto out_dec;

	buffer_a->buffers[cpu] = cpu_buffer_b;
	buffer_b->buffers[cpu] = cpu_buffer_a;

	cpu_buffer_b->buffer = buffer_a;
	cpu_buffer_a->buffer = buffer_b;

	ret = 0;

out_dec:
	atomic_dec(&cpu_buffer_a->record_disabled);
	atomic_dec(&cpu_buffer_b->record_disabled);
out:
	return ret;
}