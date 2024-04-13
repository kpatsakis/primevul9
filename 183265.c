static struct page *domain_unmap(struct dmar_domain *domain,
				 unsigned long start_pfn,
				 unsigned long last_pfn)
{
	struct page *freelist;

	BUG_ON(!domain_pfn_supported(domain, start_pfn));
	BUG_ON(!domain_pfn_supported(domain, last_pfn));
	BUG_ON(start_pfn > last_pfn);

	/* we don't need lock here; nobody else touches the iova range */
	freelist = dma_pte_clear_level(domain, agaw_to_level(domain->agaw),
				       domain->pgd, 0, start_pfn, last_pfn, NULL);

	/* free pgd */
	if (start_pfn == 0 && last_pfn == DOMAIN_MAX_PFN(domain->gaw)) {
		struct page *pgd_page = virt_to_page(domain->pgd);
		pgd_page->freelist = freelist;
		freelist = pgd_page;

		domain->pgd = NULL;
	}

	return freelist;
}