static int gup_hugepte(pte_t *ptep, unsigned long sz, unsigned long addr,
		       unsigned long end, unsigned int flags,
		       struct page **pages, int *nr)
{
	unsigned long pte_end;
	struct page *head, *page;
	pte_t pte;
	int refs;

	pte_end = (addr + sz) & ~(sz-1);
	if (pte_end < end)
		end = pte_end;

	pte = READ_ONCE(*ptep);

	if (!pte_access_permitted(pte, flags & FOLL_WRITE))
		return 0;

	/* hugepages are never "special" */
	VM_BUG_ON(!pfn_valid(pte_pfn(pte)));

	head = pte_page(pte);
	page = head + ((addr & (sz-1)) >> PAGE_SHIFT);
	refs = record_subpages(page, addr, end, pages + *nr);

	head = try_grab_compound_head(head, refs, flags);
	if (!head)
		return 0;

	if (unlikely(pte_val(pte) != pte_val(*ptep))) {
		put_compound_head(head, refs, flags);
		return 0;
	}

	*nr += refs;
	SetPageReferenced(head);
	return 1;
}