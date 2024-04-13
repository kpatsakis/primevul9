static void verify_event(struct ring_buffer_per_cpu *cpu_buffer,
			 void *event)
{
	struct buffer_page *page = cpu_buffer->commit_page;
	struct buffer_page *tail_page = READ_ONCE(cpu_buffer->tail_page);
	struct list_head *next;
	long commit, write;
	unsigned long addr = (unsigned long)event;
	bool done = false;
	int stop = 0;

	/* Make sure the event exists and is not committed yet */
	do {
		if (page == tail_page || WARN_ON_ONCE(stop++ > 100))
			done = true;
		commit = local_read(&page->page->commit);
		write = local_read(&page->write);
		if (addr >= (unsigned long)&page->page->data[commit] &&
		    addr < (unsigned long)&page->page->data[write])
			return;

		next = rb_list_head(page->list.next);
		page = list_entry(next, struct buffer_page, list);
	} while (!done);
	WARN_ON_ONCE(1);
}