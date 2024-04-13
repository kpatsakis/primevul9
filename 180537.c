static size_t intel_iommu_unmap(struct iommu_domain *domain,
				unsigned long iova, size_t size)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct page *freelist = NULL;
	unsigned long start_pfn, last_pfn;
	unsigned int npages;
	int iommu_id, level = 0;

	/* Cope with horrid API which requires us to unmap more than the
	   size argument if it happens to be a large-page mapping. */
	BUG_ON(!pfn_to_dma_pte(dmar_domain, iova >> VTD_PAGE_SHIFT, &level));

	if (size < VTD_PAGE_SIZE << level_to_offset_bits(level))
		size = VTD_PAGE_SIZE << level_to_offset_bits(level);

	start_pfn = iova >> VTD_PAGE_SHIFT;
	last_pfn = (iova + size - 1) >> VTD_PAGE_SHIFT;

	freelist = domain_unmap(dmar_domain, start_pfn, last_pfn);

	npages = last_pfn - start_pfn + 1;

	for_each_domain_iommu(iommu_id, dmar_domain)
		iommu_flush_iotlb_psi(g_iommus[iommu_id], dmar_domain,
				      start_pfn, npages, !freelist, 0);

	dma_free_pagelist(freelist);

	if (dmar_domain->max_addr == iova + size)
		dmar_domain->max_addr = iova;

	return size;
}