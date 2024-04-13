rb_decrement_entry(struct ring_buffer_per_cpu *cpu_buffer,
		   struct ring_buffer_event *event)
{
	unsigned long addr = (unsigned long)event;
	struct buffer_page *bpage = cpu_buffer->commit_page;
	struct buffer_page *start;

	addr &= PAGE_MASK;

	/* Do the likely case first */
	if (likely(bpage->page == (void *)addr)) {
		local_dec(&bpage->entries);
		return;
	}

	/*
	 * Because the commit page may be on the reader page we
	 * start with the next page and check the end loop there.
	 */
	rb_inc_page(&bpage);
	start = bpage;
	do {
		if (bpage->page == (void *)addr) {
			local_dec(&bpage->entries);
			return;
		}
		rb_inc_page(&bpage);
	} while (bpage != start);

	/* commit not part of this buffer?? */
	RB_WARN_ON(cpu_buffer, 1);
}