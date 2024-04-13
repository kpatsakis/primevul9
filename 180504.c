static int init_iommu_hw(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;

	for_each_active_iommu(iommu, drhd)
		if (iommu->qi)
			dmar_reenable_qi(iommu);

	for_each_iommu(iommu, drhd) {
		if (drhd->ignored) {
			/*
			 * we always have to disable PMRs or DMA may fail on
			 * this device
			 */
			if (force_on)
				iommu_disable_protect_mem_regions(iommu);
			continue;
		}
	
		iommu_flush_write_buffer(iommu);

		iommu_set_root_entry(iommu);

		iommu->flush.flush_context(iommu, 0, 0, 0,
					   DMA_CCMD_GLOBAL_INVL);
		iommu->flush.flush_iotlb(iommu, 0, 0, 0, DMA_TLB_GLOBAL_FLUSH);
		iommu_enable_translation(iommu);
		iommu_disable_protect_mem_regions(iommu);
	}

	return 0;
}