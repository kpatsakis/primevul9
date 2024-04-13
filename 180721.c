void free_unref_page_list(struct list_head *list)
{
	struct page *page, *next;
	unsigned long flags, pfn;

	/* Prepare pages for freeing */
	list_for_each_entry_safe(page, next, list, lru) {
		pfn = page_to_pfn(page);
		if (!free_unref_page_prepare(page, pfn))
			list_del(&page->lru);
		set_page_private(page, pfn);
	}

	local_irq_save(flags);
	list_for_each_entry_safe(page, next, list, lru) {
		unsigned long pfn = page_private(page);

		set_page_private(page, 0);
		trace_mm_page_free_batched(page);
		free_unref_page_commit(page, pfn);
	}
	local_irq_restore(flags);
}