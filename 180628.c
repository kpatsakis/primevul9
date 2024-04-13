static struct page *dma_pte_list_pagetables(struct dmar_domain *domain,
					    int level, struct dma_pte *pte,
					    struct page *freelist)
{
	struct page *pg;

	pg = pfn_to_page(dma_pte_addr(pte) >> PAGE_SHIFT);
	pg->freelist = freelist;
	freelist = pg;

	if (level == 1)
		return freelist;

	pte = page_address(pg);
	do {
		if (dma_pte_present(pte) && !dma_pte_superpage(pte))
			freelist = dma_pte_list_pagetables(domain, level - 1,
							   pte, freelist);
		pte++;
	} while (!first_pte_in_page(pte));

	return freelist;
}