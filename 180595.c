static dma_addr_t __intel_map_single(struct device *dev, phys_addr_t paddr,
				     size_t size, int dir, u64 dma_mask)
{
	struct dmar_domain *domain;
	phys_addr_t start_paddr;
	unsigned long iova_pfn;
	int prot = 0;
	int ret;
	struct intel_iommu *iommu;
	unsigned long paddr_pfn = paddr >> PAGE_SHIFT;

	BUG_ON(dir == DMA_NONE);

	if (iommu_no_mapping(dev))
		return paddr;

	domain = get_valid_domain_for_dev(dev);
	if (!domain)
		return 0;

	iommu = domain_get_iommu(domain);
	size = aligned_nrpages(paddr, size);

	iova_pfn = intel_alloc_iova(dev, domain, dma_to_mm_pfn(size), dma_mask);
	if (!iova_pfn)
		goto error;

	/*
	 * Check if DMAR supports zero-length reads on write only
	 * mappings..
	 */
	if (dir == DMA_TO_DEVICE || dir == DMA_BIDIRECTIONAL || \
			!cap_zlr(iommu->cap))
		prot |= DMA_PTE_READ;
	if (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL)
		prot |= DMA_PTE_WRITE;
	/*
	 * paddr - (paddr + size) might be partial page, we should map the whole
	 * page.  Note: if two part of one page are separately mapped, we
	 * might have two guest_addr mapping to the same host paddr, but this
	 * is not a big problem
	 */
	ret = domain_pfn_mapping(domain, mm_to_dma_pfn(iova_pfn),
				 mm_to_dma_pfn(paddr_pfn), size, prot);
	if (ret)
		goto error;

	start_paddr = (phys_addr_t)iova_pfn << PAGE_SHIFT;
	start_paddr += paddr & ~PAGE_MASK;
	return start_paddr;

error:
	if (iova_pfn)
		free_iova_fast(&domain->iovad, iova_pfn, dma_to_mm_pfn(size));
	pr_err("Device %s request: %zx@%llx dir %d --- failed\n",
		dev_name(dev), size, (unsigned long long)paddr, dir);
	return 0;
}