static void domain_update_iommu_cap(struct dmar_domain *domain)
{
	domain_update_iommu_coherency(domain);
	domain->iommu_snooping = domain_update_iommu_snooping(NULL);
	domain->iommu_superpage = domain_update_iommu_superpage(NULL);
}