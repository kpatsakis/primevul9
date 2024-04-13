static bool translation_pre_enabled(struct intel_iommu *iommu)
{
	return (iommu->flags & VTD_FLAG_TRANS_PRE_ENABLED);
}