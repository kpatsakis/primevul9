rb_num_of_entries(struct ring_buffer_per_cpu *cpu_buffer)
{
	return local_read(&cpu_buffer->entries) -
		(local_read(&cpu_buffer->overrun) + cpu_buffer->read);
}