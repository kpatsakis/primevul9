static int domain_update_iommu_superpage(struct intel_iommu *skip)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	int mask = 0xf;

	if (!intel_iommu_superpage) {
		return 0;
	}

	/* set iommu_superpage to the smallest common denominator */
	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (iommu != skip) {
			mask &= cap_super_page_val(iommu->cap);
			if (!mask)
				break;
		}
	}
	rcu_read_unlock();

	return fls(mask);
}