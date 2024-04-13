static struct page *dma_pte_clear_level(struct dmar_domain *domain, int level,
					struct dma_pte *pte, unsigned long pfn,
					unsigned long start_pfn,
					unsigned long last_pfn,
					struct page *freelist)
{
	struct dma_pte *first_pte = NULL, *last_pte = NULL;

	pfn = max(start_pfn, pfn);
	pte = &pte[pfn_level_offset(pfn, level)];

	do {
		unsigned long level_pfn;

		if (!dma_pte_present(pte))
			goto next;

		level_pfn = pfn & level_mask(level);

		/* If range covers entire pagetable, free it */
		if (start_pfn <= level_pfn &&
		    last_pfn >= level_pfn + level_size(level) - 1) {
			/* These suborbinate page tables are going away entirely. Don't
			   bother to clear them; we're just going to *free* them. */
			if (level > 1 && !dma_pte_superpage(pte))
				freelist = dma_pte_list_pagetables(domain, level - 1, pte, freelist);

			dma_clear_pte(pte);
			if (!first_pte)
				first_pte = pte;
			last_pte = pte;
		} else if (level > 1) {
			/* Recurse down into a level that isn't *entirely* obsolete */
			freelist = dma_pte_clear_level(domain, level - 1,
						       phys_to_virt(dma_pte_addr(pte)),
						       level_pfn, start_pfn, last_pfn,
						       freelist);
		}
next:
		pfn += level_size(level);
	} while (!first_pte_in_page(++pte) && pfn <= last_pfn);

	if (first_pte)
		domain_flush_cache(domain, first_pte,
				   (void *)++last_pte - (void *)first_pte);

	return freelist;
}