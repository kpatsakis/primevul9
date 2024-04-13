struct intel_iommu *domain_get_iommu(struct dmar_domain *domain)
{
	int iommu_id;

	/* si_domain and vm domain should not get here. */
	BUG_ON(domain_type_is_vm_or_si(domain));
	for_each_domain_iommu(iommu_id, domain)
		break;

	if (iommu_id < 0 || iommu_id >= g_num_of_iommus)
		return NULL;

	return g_iommus[iommu_id];
}