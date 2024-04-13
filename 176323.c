static int gup_huge_pmd(pmd_t orig, pmd_t *pmdp, unsigned long addr,
			unsigned long end, unsigned int flags,
			struct page **pages, int *nr)
{
	struct page *head, *page;
	int refs;

	if (!pmd_access_permitted(orig, flags & FOLL_WRITE))
		return 0;

	if (pmd_devmap(orig)) {
		if (unlikely(flags & FOLL_LONGTERM))
			return 0;
		return __gup_device_huge_pmd(orig, pmdp, addr, end, flags,
					     pages, nr);
	}

	page = pmd_page(orig) + ((addr & ~PMD_MASK) >> PAGE_SHIFT);
	refs = record_subpages(page, addr, end, pages + *nr);

	head = try_grab_compound_head(pmd_page(orig), refs, flags);
	if (!head)
		return 0;

	if (unlikely(pmd_val(orig) != pmd_val(*pmdp))) {
		put_compound_head(head, refs, flags);
		return 0;
	}

	*nr += refs;
	SetPageReferenced(head);
	return 1;
}