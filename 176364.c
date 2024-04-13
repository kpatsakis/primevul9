static int gup_huge_pud(pud_t orig, pud_t *pudp, unsigned long addr,
			unsigned long end, unsigned int flags,
			struct page **pages, int *nr)
{
	struct page *head, *page;
	int refs;

	if (!pud_access_permitted(orig, flags & FOLL_WRITE))
		return 0;

	if (pud_devmap(orig)) {
		if (unlikely(flags & FOLL_LONGTERM))
			return 0;
		return __gup_device_huge_pud(orig, pudp, addr, end, flags,
					     pages, nr);
	}

	page = pud_page(orig) + ((addr & ~PUD_MASK) >> PAGE_SHIFT);
	refs = record_subpages(page, addr, end, pages + *nr);

	head = try_grab_compound_head(pud_page(orig), refs, flags);
	if (!head)
		return 0;

	if (unlikely(pud_val(orig) != pud_val(*pudp))) {
		put_compound_head(head, refs, flags);
		return 0;
	}

	*nr += refs;
	SetPageReferenced(head);
	return 1;
}