static int __rb_allocate_pages(long nr_pages, struct list_head *pages, int cpu)
{
	struct buffer_page *bpage, *tmp;
	long i;

	for (i = 0; i < nr_pages; i++) {
		struct page *page;
		/*
		 * __GFP_NORETRY flag makes sure that the allocation fails
		 * gracefully without invoking oom-killer and the system is
		 * not destabilized.
		 */
		bpage = kzalloc_node(ALIGN(sizeof(*bpage), cache_line_size()),
				    GFP_KERNEL | __GFP_NORETRY,
				    cpu_to_node(cpu));
		if (!bpage)
			goto free_pages;

		list_add(&bpage->list, pages);

		page = alloc_pages_node(cpu_to_node(cpu),
					GFP_KERNEL | __GFP_NORETRY, 0);
		if (!page)
			goto free_pages;
		bpage->page = page_address(page);
		rb_init_page(bpage->page);
	}

	return 0;

free_pages:
	list_for_each_entry_safe(bpage, tmp, pages, list) {
		list_del_init(&bpage->list);
		free_buffer_page(bpage);
	}

	return -ENOMEM;
}