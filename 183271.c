static void iommu_set_root_entry(struct intel_iommu *iommu)
{
	u64 addr;
	u32 sts;
	unsigned long flag;

	addr = virt_to_phys(iommu->root_entry);
	if (sm_supported(iommu))
		addr |= DMA_RTADDR_SMT;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	dmar_writeq(iommu->reg + DMAR_RTADDR_REG, addr);

	writel(iommu->gcmd | DMA_GCMD_SRTP, iommu->reg + DMAR_GCMD_REG);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_GSTS_REG,
		      readl, (sts & DMA_GSTS_RTPS), sts);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
}