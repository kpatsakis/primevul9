static void domain_update_iommu_coherency(struct dmar_domain *domain)
{
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	bool found = false;
	int i;

	domain->iommu_coherency = 1;

	for_each_domain_iommu(i, domain) {
		found = true;
		if (!ecap_coherent(g_iommus[i]->ecap)) {
			domain->iommu_coherency = 0;
			break;
		}
	}
	if (found)
		return;

	/* No hardware attached; use lowest common denominator */
	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (!ecap_coherent(iommu->ecap)) {
			domain->iommu_coherency = 0;
			break;
		}
	}
	rcu_read_unlock();
}