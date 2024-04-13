static void intel_iommu_domain_free(struct iommu_domain *domain)
{
	domain_exit(to_dmar_domain(domain));
}