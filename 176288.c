static void hpage_pincount_add(struct page *page, int refs)
{
	VM_BUG_ON_PAGE(!hpage_pincount_available(page), page);
	VM_BUG_ON_PAGE(page != compound_head(page), page);

	atomic_add(refs, compound_pincount_ptr(page));
}