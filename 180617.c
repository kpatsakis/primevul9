static int intel_map_sg(struct device *dev, struct scatterlist *sglist, int nelems,
			enum dma_data_direction dir, unsigned long attrs)
{
	int i;
	struct dmar_domain *domain;
	size_t size = 0;
	int prot = 0;
	unsigned long iova_pfn;
	int ret;
	struct scatterlist *sg;
	unsigned long start_vpfn;
	struct intel_iommu *iommu;

	BUG_ON(dir == DMA_NONE);
	if (iommu_no_mapping(dev))
		return intel_nontranslate_map_sg(dev, sglist, nelems, dir);

	domain = get_valid_domain_for_dev(dev);
	if (!domain)
		return 0;

	iommu = domain_get_iommu(domain);

	for_each_sg(sglist, sg, nelems, i)
		size += aligned_nrpages(sg->offset, sg->length);

	iova_pfn = intel_alloc_iova(dev, domain, dma_to_mm_pfn(size),
				*dev->dma_mask);
	if (!iova_pfn) {
		sglist->dma_length = 0;
		return 0;
	}

	/*
	 * Check if DMAR supports zero-length reads on write only
	 * mappings..
	 */
	if (dir == DMA_TO_DEVICE || dir == DMA_BIDIRECTIONAL || \
			!cap_zlr(iommu->cap))
		prot |= DMA_PTE_READ;
	if (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL)
		prot |= DMA_PTE_WRITE;

	start_vpfn = mm_to_dma_pfn(iova_pfn);

	ret = domain_sg_mapping(domain, start_vpfn, sglist, size, prot);
	if (unlikely(ret)) {
		dma_pte_free_pagetable(domain, start_vpfn,
				       start_vpfn + size - 1,
				       agaw_to_level(domain->agaw) + 1);
		free_iova_fast(&domain->iovad, iova_pfn, dma_to_mm_pfn(size));
		return 0;
	}

	return nelems;
}