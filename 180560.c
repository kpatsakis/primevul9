static void iommu_flush_iova(struct iova_domain *iovad)
{
	struct dmar_domain *domain;
	int idx;

	domain = container_of(iovad, struct dmar_domain, iovad);

	for_each_domain_iommu(idx, domain) {
		struct intel_iommu *iommu = g_iommus[idx];
		u16 did = domain->iommu_did[iommu->seq_id];

		iommu->flush.flush_iotlb(iommu, did, 0, 0, DMA_TLB_DSI_FLUSH);

		if (!cap_caching_mode(iommu->cap))
			iommu_flush_dev_iotlb(get_iommu_domain(iommu, did),
					      0, MAX_AGAW_PFN_WIDTH);
	}
}