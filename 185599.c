static int __rb_allocate_pages(struct ring_buffer_per_cpu *cpu_buffer,
		long nr_pages, struct list_head *pages)
{
	struct buffer_page *bpage, *tmp;
	bool user_thread = current->mm != NULL;
	gfp_t mflags;
	long i;

	/*
	 * Check if the available memory is there first.
	 * Note, si_mem_available() only gives us a rough estimate of available
	 * memory. It may not be accurate. But we don't care, we just want
	 * to prevent doing any allocation when it is obvious that it is
	 * not going to succeed.
	 */
	i = si_mem_available();
	if (i < nr_pages)
		return -ENOMEM;

	/*
	 * __GFP_RETRY_MAYFAIL flag makes sure that the allocation fails
	 * gracefully without invoking oom-killer and the system is not
	 * destabilized.
	 */
	mflags = GFP_KERNEL | __GFP_RETRY_MAYFAIL;

	/*
	 * If a user thread allocates too much, and si_mem_available()
	 * reports there's enough memory, even though there is not.
	 * Make sure the OOM killer kills this thread. This can happen
	 * even with RETRY_MAYFAIL because another task may be doing
	 * an allocation after this task has taken all memory.
	 * This is the task the OOM killer needs to take out during this
	 * loop, even if it was triggered by an allocation somewhere else.
	 */
	if (user_thread)
		set_current_oom_origin();
	for (i = 0; i < nr_pages; i++) {
		struct page *page;

		bpage = kzalloc_node(ALIGN(sizeof(*bpage), cache_line_size()),
				    mflags, cpu_to_node(cpu_buffer->cpu));
		if (!bpage)
			goto free_pages;

		rb_check_bpage(cpu_buffer, bpage);

		list_add(&bpage->list, pages);

		page = alloc_pages_node(cpu_to_node(cpu_buffer->cpu), mflags, 0);
		if (!page)
			goto free_pages;
		bpage->page = page_address(page);
		rb_init_page(bpage->page);

		if (user_thread && fatal_signal_pending(current))
			goto free_pages;
	}
	if (user_thread)
		clear_current_oom_origin();

	return 0;

free_pages:
	list_for_each_entry_safe(bpage, tmp, pages, list) {
		list_del_init(&bpage->list);
		free_buffer_page(bpage);
	}
	if (user_thread)
		clear_current_oom_origin();

	return -ENOMEM;
}