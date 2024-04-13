rb_commit_index(struct ring_buffer_per_cpu *cpu_buffer)
{
	return rb_page_commit(cpu_buffer->commit_page);
}