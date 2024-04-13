static void rb_update_pages(struct ring_buffer_per_cpu *cpu_buffer)
{
	int success;

	if (cpu_buffer->nr_pages_to_update > 0)
		success = rb_insert_pages(cpu_buffer);
	else
		success = rb_remove_pages(cpu_buffer,
					-cpu_buffer->nr_pages_to_update);

	if (success)
		cpu_buffer->nr_pages += cpu_buffer->nr_pages_to_update;
}