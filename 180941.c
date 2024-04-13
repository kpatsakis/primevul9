void free_unref_page(struct page *page)
{
	unsigned long flags;
	unsigned long pfn = page_to_pfn(page);

	if (!free_unref_page_prepare(page, pfn))
		return;

	local_irq_save(flags);
	free_unref_page_commit(page, pfn);
	local_irq_restore(flags);
}