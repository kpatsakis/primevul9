static struct dmar_domain *alloc_domain(int flags)
{
	struct dmar_domain *domain;

	domain = alloc_domain_mem();
	if (!domain)
		return NULL;

	memset(domain, 0, sizeof(*domain));
	domain->nid = -1;
	domain->flags = flags;
	domain->has_iotlb_device = false;
	INIT_LIST_HEAD(&domain->devices);

	return domain;
}