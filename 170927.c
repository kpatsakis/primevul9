static int rb_lost_events(struct ring_buffer_per_cpu *cpu_buffer)
{
	return cpu_buffer->lost_events;
}