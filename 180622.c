static phys_addr_t intel_iommu_iova_to_phys(struct iommu_domain *domain,
					    dma_addr_t iova)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct dma_pte *pte;
	int level = 0;
	u64 phys = 0;

	pte = pfn_to_dma_pte(dmar_domain, iova >> VTD_PAGE_SHIFT, &level);
	if (pte)
		phys = dma_pte_addr(pte);

	return phys;
}