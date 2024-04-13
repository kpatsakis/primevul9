static struct dmar_domain *to_dmar_domain(struct iommu_domain *dom)
{
	return container_of(dom, struct dmar_domain, domain);
}