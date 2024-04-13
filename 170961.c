unsigned long ring_buffer_size(struct ring_buffer *buffer, int cpu)
{
	/*
	 * Earlier, this method returned
	 *	BUF_PAGE_SIZE * buffer->nr_pages
	 * Since the nr_pages field is now removed, we have converted this to
	 * return the per cpu buffer value.
	 */
	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	return BUF_PAGE_SIZE * buffer->buffers[cpu]->nr_pages;
}