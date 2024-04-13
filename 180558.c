static int iommu_domain_identity_map(struct dmar_domain *domain,
				     unsigned long long start,
				     unsigned long long end)
{
	unsigned long first_vpfn = start >> VTD_PAGE_SHIFT;
	unsigned long last_vpfn = end >> VTD_PAGE_SHIFT;

	if (!reserve_iova(&domain->iovad, dma_to_mm_pfn(first_vpfn),
			  dma_to_mm_pfn(last_vpfn))) {
		pr_err("Reserving iova failed\n");
		return -ENOMEM;
	}

	pr_debug("Mapping reserved region %llx-%llx\n", start, end);
	/*
	 * RMRR range might have overlap with physical memory range,
	 * clear it first
	 */
	dma_pte_clear_range(domain, first_vpfn, last_vpfn);

	return __domain_mapping(domain, first_vpfn, NULL,
				first_vpfn, last_vpfn - first_vpfn + 1,
				DMA_PTE_READ|DMA_PTE_WRITE);
}