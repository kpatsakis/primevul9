int iommu_calculate_agaw(struct intel_iommu *iommu)
{
	return __iommu_calculate_agaw(iommu, DEFAULT_DOMAIN_ADDRESS_WIDTH);
}