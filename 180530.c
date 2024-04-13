static struct iommu_domain *intel_iommu_domain_alloc(unsigned type)
{
	struct dmar_domain *dmar_domain;
	struct iommu_domain *domain;

	if (type != IOMMU_DOMAIN_UNMANAGED)
		return NULL;

	dmar_domain = alloc_domain(DOMAIN_FLAG_VIRTUAL_MACHINE);
	if (!dmar_domain) {
		pr_err("Can't allocate dmar_domain\n");
		return NULL;
	}
	if (md_domain_init(dmar_domain, DEFAULT_DOMAIN_ADDRESS_WIDTH)) {
		pr_err("Domain initialization failed\n");
		domain_exit(dmar_domain);
		return NULL;
	}
	domain_update_iommu_cap(dmar_domain);

	domain = &dmar_domain->domain;
	domain->geometry.aperture_start = 0;
	domain->geometry.aperture_end   = __DOMAIN_MAX_ADDR(dmar_domain->gaw);
	domain->geometry.force_aperture = true;

	return domain;
}