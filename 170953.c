rb_insert_pages(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *pages = &cpu_buffer->new_pages;
	int retries, success;

	raw_spin_lock_irq(&cpu_buffer->reader_lock);
	/*
	 * We are holding the reader lock, so the reader page won't be swapped
	 * in the ring buffer. Now we are racing with the writer trying to
	 * move head page and the tail page.
	 * We are going to adapt the reader page update process where:
	 * 1. We first splice the start and end of list of new pages between
	 *    the head page and its previous page.
	 * 2. We cmpxchg the prev_page->next to point from head page to the
	 *    start of new pages list.
	 * 3. Finally, we update the head->prev to the end of new list.
	 *
	 * We will try this process 10 times, to make sure that we don't keep
	 * spinning.
	 */
	retries = 10;
	success = 0;
	while (retries--) {
		struct list_head *head_page, *prev_page, *r;
		struct list_head *last_page, *first_page;
		struct list_head *head_page_with_bit;

		head_page = &rb_set_head_page(cpu_buffer)->list;
		if (!head_page)
			break;
		prev_page = head_page->prev;

		first_page = pages->next;
		last_page  = pages->prev;

		head_page_with_bit = (struct list_head *)
				     ((unsigned long)head_page | RB_PAGE_HEAD);

		last_page->next = head_page_with_bit;
		first_page->prev = prev_page;

		r = cmpxchg(&prev_page->next, head_page_with_bit, first_page);

		if (r == head_page_with_bit) {
			/*
			 * yay, we replaced the page pointer to our new list,
			 * now, we just have to update to head page's prev
			 * pointer to point to end of list
			 */
			head_page->prev = last_page;
			success = 1;
			break;
		}
	}

	if (success)
		INIT_LIST_HEAD(pages);
	/*
	 * If we weren't successful in adding in new pages, warn and stop
	 * tracing
	 */
	RB_WARN_ON(cpu_buffer, !success);
	raw_spin_unlock_irq(&cpu_buffer->reader_lock);

	/* free pages if they weren't inserted */
	if (!success) {
		struct buffer_page *bpage, *tmp;
		list_for_each_entry_safe(bpage, tmp, &cpu_buffer->new_pages,
					 list) {
			list_del_init(&bpage->list);
			free_buffer_page(bpage);
		}
	}
	return success;
}