static int md_domain_init(struct dmar_domain *domain, int guest_width)
{
	int adjust_width;

	init_iova_domain(&domain->iovad, VTD_PAGE_SIZE, IOVA_START_PFN);
	domain_reserve_special_ranges(domain);

	/* calculate AGAW */
	domain->gaw = guest_width;
	adjust_width = guestwidth_to_adjustwidth(guest_width);
	domain->agaw = width_to_agaw(adjust_width);

	domain->iommu_coherency = 0;
	domain->iommu_snooping = 0;
	domain->iommu_superpage = 0;
	domain->max_addr = 0;

	/* always allocate the top pgd */
	domain->pgd = (struct dma_pte *)alloc_pgtable_page(domain->nid);
	if (!domain->pgd)
		return -ENOMEM;
	domain_flush_cache(domain, domain->pgd, PAGE_SIZE);
	return 0;
}