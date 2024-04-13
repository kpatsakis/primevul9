static void set_iommu_domain(struct intel_iommu *iommu, u16 did,
			     struct dmar_domain *domain)
{
	struct dmar_domain **domains;
	int idx = did >> 8;

	if (!iommu->domains[idx]) {
		size_t size = 256 * sizeof(struct dmar_domain *);
		iommu->domains[idx] = kzalloc(size, GFP_ATOMIC);
	}

	domains = iommu->domains[idx];
	if (WARN_ON(!domains))
		return;
	else
		domains[did & 0xff] = domain;
}