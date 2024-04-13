trace_recursive_lock(struct ring_buffer_per_cpu *cpu_buffer)
{
	unsigned int val = cpu_buffer->current_context;
	int bit;

	if (in_interrupt()) {
		if (in_nmi())
			bit = RB_CTX_NMI;
		else if (in_irq())
			bit = RB_CTX_IRQ;
		else
			bit = RB_CTX_SOFTIRQ;
	} else
		bit = RB_CTX_NORMAL;

	if (unlikely(val & (1 << bit)))
		return 1;

	val |= (1 << bit);
	cpu_buffer->current_context = val;

	return 0;
}