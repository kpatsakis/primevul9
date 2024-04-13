static void rb_commit(struct ring_buffer_per_cpu *cpu_buffer,
		      struct ring_buffer_event *event)
{
	local_inc(&cpu_buffer->entries);
	rb_update_write_stamp(cpu_buffer, event);
	rb_end_commit(cpu_buffer);
}