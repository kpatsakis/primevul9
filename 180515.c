static void iommu_flush_iotlb_psi(struct intel_iommu *iommu,
				  struct dmar_domain *domain,
				  unsigned long pfn, unsigned int pages,
				  int ih, int map)
{
	unsigned int mask = ilog2(__roundup_pow_of_two(pages));
	uint64_t addr = (uint64_t)pfn << VTD_PAGE_SHIFT;
	u16 did = domain->iommu_did[iommu->seq_id];

	BUG_ON(pages == 0);

	if (ih)
		ih = 1 << 6;
	/*
	 * Fallback to domain selective flush if no PSI support or the size is
	 * too big.
	 * PSI requires page size to be 2 ^ x, and the base address is naturally
	 * aligned to the size
	 */
	if (!cap_pgsel_inv(iommu->cap) || mask > cap_max_amask_val(iommu->cap))
		iommu->flush.flush_iotlb(iommu, did, 0, 0,
						DMA_TLB_DSI_FLUSH);
	else
		iommu->flush.flush_iotlb(iommu, did, addr | ih, mask,
						DMA_TLB_PSI_FLUSH);

	/*
	 * In caching mode, changes of pages from non-present to present require
	 * flush. However, device IOTLB doesn't need to be flushed in this case.
	 */
	if (!cap_caching_mode(iommu->cap) || !map)
		iommu_flush_dev_iotlb(domain, addr, mask);
}