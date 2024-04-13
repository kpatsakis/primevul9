trace_recursive_lock(struct ring_buffer_per_cpu *cpu_buffer)
{
	unsigned int val = cpu_buffer->current_context;
	unsigned long pc = preempt_count();
	int bit;

	if (!(pc & (NMI_MASK | HARDIRQ_MASK | SOFTIRQ_OFFSET)))
		bit = RB_CTX_NORMAL;
	else
		bit = pc & NMI_MASK ? RB_CTX_NMI :
			pc & HARDIRQ_MASK ? RB_CTX_IRQ : RB_CTX_SOFTIRQ;

	if (unlikely(val & (1 << (bit + cpu_buffer->nest)))) {
		/*
		 * It is possible that this was called by transitioning
		 * between interrupt context, and preempt_count() has not
		 * been updated yet. In this case, use the TRANSITION bit.
		 */
		bit = RB_CTX_TRANSITION;
		if (val & (1 << (bit + cpu_buffer->nest))) {
			do_ring_buffer_record_recursion();
			return 1;
		}
	}

	val |= (1 << (bit + cpu_buffer->nest));
	cpu_buffer->current_context = val;

	return 0;
}