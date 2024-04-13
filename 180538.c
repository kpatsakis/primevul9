static unsigned long intel_alloc_iova(struct device *dev,
				     struct dmar_domain *domain,
				     unsigned long nrpages, uint64_t dma_mask)
{
	unsigned long iova_pfn = 0;

	/* Restrict dma_mask to the width that the iommu can handle */
	dma_mask = min_t(uint64_t, DOMAIN_MAX_ADDR(domain->gaw), dma_mask);
	/* Ensure we reserve the whole size-aligned region */
	nrpages = __roundup_pow_of_two(nrpages);

	if (!dmar_forcedac && dma_mask > DMA_BIT_MASK(32)) {
		/*
		 * First try to allocate an io virtual address in
		 * DMA_BIT_MASK(32) and if that fails then try allocating
		 * from higher range
		 */
		iova_pfn = alloc_iova_fast(&domain->iovad, nrpages,
					   IOVA_PFN(DMA_BIT_MASK(32)), false);
		if (iova_pfn)
			return iova_pfn;
	}
	iova_pfn = alloc_iova_fast(&domain->iovad, nrpages,
				   IOVA_PFN(dma_mask), true);
	if (unlikely(!iova_pfn)) {
		pr_err("Allocating %ld-page iova for %s failed",
		       nrpages, dev_name(dev));
		return 0;
	}

	return iova_pfn;
}