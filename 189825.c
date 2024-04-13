static int pagemap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			     void *private)
{
	struct pagemapread *pm = private;
	pte_t *pte;
	int err = 0;

	for (; addr != end; addr += PAGE_SIZE) {
		u64 pfn = PM_NOT_PRESENT;
		pte = pte_offset_map(pmd, addr);
		if (is_swap_pte(*pte))
			pfn = swap_pte_to_pagemap_entry(*pte);
		else if (pte_present(*pte))
			pfn = pte_pfn(*pte);
		/* unmap so we're not in atomic when we copy to userspace */
		pte_unmap(pte);
		err = add_to_pagemap(addr, pfn, pm);
		if (err)
			return err;
	}

	cond_resched();

	return err;
}