static int me_huge_page(struct page *p, unsigned long pfn)
{
	int res = 0;
	struct page *hpage = compound_head(p);

	if (!PageHuge(hpage))
		return MF_DELAYED;

	/*
	 * We can safely recover from error on free or reserved (i.e.
	 * not in-use) hugepage by dequeuing it from freelist.
	 * To check whether a hugepage is in-use or not, we can't use
	 * page->lru because it can be used in other hugepage operations,
	 * such as __unmap_hugepage_range() and gather_surplus_pages().
	 * So instead we use page_mapping() and PageAnon().
	 */
	if (!(page_mapping(hpage) || PageAnon(hpage))) {
		res = dequeue_hwpoisoned_huge_page(hpage);
		if (!res)
			return MF_RECOVERED;
	}
	return MF_DELAYED;
}