static inline void __mapping_notify_one(struct intel_iommu *iommu,
					struct dmar_domain *domain,
					unsigned long pfn, unsigned int pages)
{
	/* It's a non-present to present mapping. Only flush if caching mode */
	if (cap_caching_mode(iommu->cap))
		iommu_flush_iotlb_psi(iommu, domain, pfn, pages, 0, 1);
	else
		iommu_flush_write_buffer(iommu);
}