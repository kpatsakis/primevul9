static void init_translation_status(struct intel_iommu *iommu)
{
	u32 gsts;

	gsts = readl(iommu->reg + DMAR_GSTS_REG);
	if (gsts & DMA_GSTS_TES)
		iommu->flags |= VTD_FLAG_TRANS_PRE_ENABLED;
}