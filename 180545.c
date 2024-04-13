static void intel_disable_iommus(void)
{
	struct intel_iommu *iommu = NULL;
	struct dmar_drhd_unit *drhd;

	for_each_iommu(iommu, drhd)
		iommu_disable_translation(iommu);
}