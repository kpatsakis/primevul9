static void clear_translation_pre_enabled(struct intel_iommu *iommu)
{
	iommu->flags &= ~VTD_FLAG_TRANS_PRE_ENABLED;
}