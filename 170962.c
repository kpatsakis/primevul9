trace_recursive_unlock(struct ring_buffer_per_cpu *cpu_buffer)
{
	cpu_buffer->current_context &= cpu_buffer->current_context - 1;
}