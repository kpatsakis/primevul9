static void iommu_enable_translation(struct intel_iommu *iommu)
{
	u32 sts;
	unsigned long flags;

	raw_spin_lock_irqsave(&iommu->register_lock, flags);
	iommu->gcmd |= DMA_GCMD_TE;
	writel(iommu->gcmd, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (sts & DMA_GSTS_TES), sts);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flags);
}