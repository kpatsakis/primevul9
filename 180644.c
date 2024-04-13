static bool intel_iommu_capable(enum iommu_cap cap)
{
	if (cap == IOMMU_CAP_CACHE_COHERENCY)
		return domain_update_iommu_snooping(NULL) == 1;
	if (cap == IOMMU_CAP_INTR_REMAP)
		return irq_remapping_enabled == 1;

	return false;
}