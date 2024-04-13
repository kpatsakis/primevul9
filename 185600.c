rb_remove_pages(struct ring_buffer_per_cpu *cpu_buffer, unsigned long nr_pages)
{
	struct list_head *tail_page, *to_remove, *next_page;
	struct buffer_page *to_remove_page, *tmp_iter_page;
	struct buffer_page *last_page, *first_page;
	unsigned long nr_removed;
	unsigned long head_bit;
	int page_entries;

	head_bit = 0;

	raw_spin_lock_irq(&cpu_buffer->reader_lock);
	atomic_inc(&cpu_buffer->record_disabled);
	/*
	 * We don't race with the readers since we have acquired the reader
	 * lock. We also don't race with writers after disabling recording.
	 * This makes it easy to figure out the first and the last page to be
	 * removed from the list. We unlink all the pages in between including
	 * the first and last pages. This is done in a busy loop so that we
	 * lose the least number of traces.
	 * The pages are freed after we restart recording and unlock readers.
	 */
	tail_page = &cpu_buffer->tail_page->list;

	/*
	 * tail page might be on reader page, we remove the next page
	 * from the ring buffer
	 */
	if (cpu_buffer->tail_page == cpu_buffer->reader_page)
		tail_page = rb_list_head(tail_page->next);
	to_remove = tail_page;

	/* start of pages to remove */
	first_page = list_entry(rb_list_head(to_remove->next),
				struct buffer_page, list);

	for (nr_removed = 0; nr_removed < nr_pages; nr_removed++) {
		to_remove = rb_list_head(to_remove)->next;
		head_bit |= (unsigned long)to_remove & RB_PAGE_HEAD;
	}

	next_page = rb_list_head(to_remove)->next;

	/*
	 * Now we remove all pages between tail_page and next_page.
	 * Make sure that we have head_bit value preserved for the
	 * next page
	 */
	tail_page->next = (struct list_head *)((unsigned long)next_page |
						head_bit);
	next_page = rb_list_head(next_page);
	next_page->prev = tail_page;

	/* make sure pages points to a valid page in the ring buffer */
	cpu_buffer->pages = next_page;

	/* update head page */
	if (head_bit)
		cpu_buffer->head_page = list_entry(next_page,
						struct buffer_page, list);

	/*
	 * change read pointer to make sure any read iterators reset
	 * themselves
	 */
	cpu_buffer->read = 0;

	/* pages are removed, resume tracing and then free the pages */
	atomic_dec(&cpu_buffer->record_disabled);
	raw_spin_unlock_irq(&cpu_buffer->reader_lock);

	RB_WARN_ON(cpu_buffer, list_empty(cpu_buffer->pages));

	/* last buffer page to remove */
	last_page = list_entry(rb_list_head(to_remove), struct buffer_page,
				list);
	tmp_iter_page = first_page;

	do {
		cond_resched();

		to_remove_page = tmp_iter_page;
		rb_inc_page(&tmp_iter_page);

		/* update the counters */
		page_entries = rb_page_entries(to_remove_page);
		if (page_entries) {
			/*
			 * If something was added to this page, it was full
			 * since it is not the tail page. So we deduct the
			 * bytes consumed in ring buffer from here.
			 * Increment overrun to account for the lost events.
			 */
			local_add(page_entries, &cpu_buffer->overrun);
			local_sub(BUF_PAGE_SIZE, &cpu_buffer->entries_bytes);
		}

		/*
		 * We have already removed references to this list item, just
		 * free up the buffer_page and its page
		 */
		free_buffer_page(to_remove_page);
		nr_removed--;

	} while (to_remove_page != last_page);

	RB_WARN_ON(cpu_buffer, nr_removed);

	return nr_removed == 0;
}