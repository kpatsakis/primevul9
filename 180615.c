static void iommu_resume(void)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu = NULL;
	unsigned long flag;

	if (init_iommu_hw()) {
		if (force_on)
			panic("tboot: IOMMU setup failed, DMAR can not resume!\n");
		else
			WARN(1, "IOMMU setup failed, DMAR can not resume!\n");
		return;
	}

	for_each_active_iommu(iommu, drhd) {

		raw_spin_lock_irqsave(&iommu->register_lock, flag);

		writel(iommu->iommu_state[SR_DMAR_FECTL_REG],
			iommu->reg + DMAR_FECTL_REG);
		writel(iommu->iommu_state[SR_DMAR_FEDATA_REG],
			iommu->reg + DMAR_FEDATA_REG);
		writel(iommu->iommu_state[SR_DMAR_FEADDR_REG],
			iommu->reg + DMAR_FEADDR_REG);
		writel(iommu->iommu_state[SR_DMAR_FEUADDR_REG],
			iommu->reg + DMAR_FEUADDR_REG);

		raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
	}

	for_each_active_iommu(iommu, drhd)
		kfree(iommu->iommu_state);
}