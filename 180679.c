static void iommu_flush_all(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;

	for_each_active_iommu(iommu, drhd) {
		iommu->flush.flush_context(iommu, 0, 0, 0,
					   DMA_CCMD_GLOBAL_INVL);
		iommu->flush.flush_iotlb(iommu, 0, 0, 0,
					 DMA_TLB_GLOBAL_FLUSH);
	}
}