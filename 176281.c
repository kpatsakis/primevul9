static int gup_pte_range(pmd_t pmd, unsigned long addr, unsigned long end,
			 unsigned int flags, struct page **pages, int *nr)
{
	struct dev_pagemap *pgmap = NULL;
	int nr_start = *nr, ret = 0;
	pte_t *ptep, *ptem;

	ptem = ptep = pte_offset_map(&pmd, addr);
	do {
		pte_t pte = gup_get_pte(ptep);
		struct page *head, *page;

		/*
		 * Similar to the PMD case below, NUMA hinting must take slow
		 * path using the pte_protnone check.
		 */
		if (pte_protnone(pte))
			goto pte_unmap;

		if (!pte_access_permitted(pte, flags & FOLL_WRITE))
			goto pte_unmap;

		if (pte_devmap(pte)) {
			if (unlikely(flags & FOLL_LONGTERM))
				goto pte_unmap;

			pgmap = get_dev_pagemap(pte_pfn(pte), pgmap);
			if (unlikely(!pgmap)) {
				undo_dev_pagemap(nr, nr_start, flags, pages);
				goto pte_unmap;
			}
		} else if (pte_special(pte))
			goto pte_unmap;

		VM_BUG_ON(!pfn_valid(pte_pfn(pte)));
		page = pte_page(pte);

		head = try_grab_compound_head(page, 1, flags);
		if (!head)
			goto pte_unmap;

		if (unlikely(pte_val(pte) != pte_val(*ptep))) {
			put_compound_head(head, 1, flags);
			goto pte_unmap;
		}

		VM_BUG_ON_PAGE(compound_head(page) != head, page);

		/*
		 * We need to make the page accessible if and only if we are
		 * going to access its content (the FOLL_PIN case).  Please
		 * see Documentation/core-api/pin_user_pages.rst for
		 * details.
		 */
		if (flags & FOLL_PIN) {
			ret = arch_make_page_accessible(page);
			if (ret) {
				unpin_user_page(page);
				goto pte_unmap;
			}
		}
		SetPageReferenced(page);
		pages[*nr] = page;
		(*nr)++;

	} while (ptep++, addr += PAGE_SIZE, addr != end);

	ret = 1;

pte_unmap:
	if (pgmap)
		put_dev_pagemap(pgmap);
	pte_unmap(ptem);
	return ret;
}