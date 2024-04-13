static bool rb_per_cpu_empty(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct buffer_page *reader = cpu_buffer->reader_page;
	struct buffer_page *head = rb_set_head_page(cpu_buffer);
	struct buffer_page *commit = cpu_buffer->commit_page;

	/* In case of error, head will be NULL */
	if (unlikely(!head))
		return true;

	/* Reader should exhaust content in reader page */
	if (reader->read != rb_page_commit(reader))
		return false;

	/*
	 * If writers are committing on the reader page, knowing all
	 * committed content has been read, the ring buffer is empty.
	 */
	if (commit == reader)
		return true;

	/*
	 * If writers are committing on a page other than reader page
	 * and head page, there should always be content to read.
	 */
	if (commit != head)
		return false;

	/*
	 * Writers are committing on the head page, we just need
	 * to care about there're committed data, and the reader will
	 * swap reader page with head page when it is to read data.
	 */
	return rb_page_commit(commit) == 0;
}