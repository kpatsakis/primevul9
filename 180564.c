static int iommu_suspend(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;
	unsigned long flag;

	for_each_active_iommu(iommu, drhd) {
		iommu->iommu_state = kcalloc(MAX_SR_DMAR_REGS, sizeof(u32),
						 GFP_ATOMIC);
		if (!iommu->iommu_state)
			goto nomem;
	}

	iommu_flush_all();

	for_each_active_iommu(iommu, drhd) {
		iommu_disable_translation(iommu);

		raw_spin_lock_irqsave(&iommu->register_lock, flag);

		iommu->iommu_state[SR_DMAR_FECTL_REG] =
			readl(iommu->reg + DMAR_FECTL_REG);
		iommu->iommu_state[SR_DMAR_FEDATA_REG] =
			readl(iommu->reg + DMAR_FEDATA_REG);
		iommu->iommu_state[SR_DMAR_FEADDR_REG] =
			readl(iommu->reg + DMAR_FEADDR_REG);
		iommu->iommu_state[SR_DMAR_FEUADDR_REG] =
			readl(iommu->reg + DMAR_FEUADDR_REG);

		raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
	}
	return 0;

nomem:
	for_each_active_iommu(iommu, drhd)
		kfree(iommu->iommu_state);

	return -ENOMEM;
}