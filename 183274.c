static struct dma_pte *pfn_to_dma_pte(struct dmar_domain *domain,
				      unsigned long pfn, int *target_level)
{
	struct dma_pte *parent, *pte;
	int level = agaw_to_level(domain->agaw);
	int offset;

	BUG_ON(!domain->pgd);

	if (!domain_pfn_supported(domain, pfn))
		/* Address beyond IOMMU's addressing capabilities. */
		return NULL;

	parent = domain->pgd;

	while (1) {
		void *tmp_page;

		offset = pfn_level_offset(pfn, level);
		pte = &parent[offset];
		if (!*target_level && (dma_pte_superpage(pte) || !dma_pte_present(pte)))
			break;
		if (level == *target_level)
			break;

		if (!dma_pte_present(pte)) {
			uint64_t pteval;

			tmp_page = alloc_pgtable_page(domain->nid);

			if (!tmp_page)
				return NULL;

			domain_flush_cache(domain, tmp_page, VTD_PAGE_SIZE);
			pteval = ((uint64_t)virt_to_dma_pfn(tmp_page) << VTD_PAGE_SHIFT) | DMA_PTE_READ | DMA_PTE_WRITE;
			if (cmpxchg64(&pte->val, 0ULL, pteval))
				/* Someone else set it while we were thinking; use theirs. */
				free_pgtable_page(tmp_page);
			else
				domain_flush_cache(domain, pte, sizeof(*pte));
		}
		if (level == 1)
			break;

		parent = phys_to_virt(dma_pte_addr(pte));
		level--;
	}

	if (!*target_level)
		*target_level = level;

	return pte;
}