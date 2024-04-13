static void intel_unmap(struct device *dev, dma_addr_t dev_addr, size_t size)
{
	struct dmar_domain *domain;
	unsigned long start_pfn, last_pfn;
	unsigned long nrpages;
	unsigned long iova_pfn;
	struct intel_iommu *iommu;
	struct page *freelist;

	if (iommu_no_mapping(dev))
		return;

	domain = find_domain(dev);
	BUG_ON(!domain);

	iommu = domain_get_iommu(domain);

	iova_pfn = IOVA_PFN(dev_addr);

	nrpages = aligned_nrpages(dev_addr, size);
	start_pfn = mm_to_dma_pfn(iova_pfn);
	last_pfn = start_pfn + nrpages - 1;

	dev_dbg(dev, "Device unmapping: pfn %lx-%lx\n", start_pfn, last_pfn);

	freelist = domain_unmap(domain, start_pfn, last_pfn);

	if (intel_iommu_strict) {
		iommu_flush_iotlb_psi(iommu, domain, start_pfn,
				      nrpages, !freelist, 0);
		/* free iova */
		free_iova_fast(&domain->iovad, iova_pfn, dma_to_mm_pfn(nrpages));
		dma_free_pagelist(freelist);
	} else {
		queue_iova(&domain->iovad, iova_pfn, nrpages,
			   (unsigned long)freelist);
		/*
		 * queue up the release of the unmap to save the 1/6th of the
		 * cpu used up by the iotlb flush operation...
		 */
	}
}