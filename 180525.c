static int iommu_prepare_identity_map(struct device *dev,
				      unsigned long long start,
				      unsigned long long end)
{
	struct dmar_domain *domain;
	int ret;

	domain = get_domain_for_dev(dev, DEFAULT_DOMAIN_ADDRESS_WIDTH);
	if (!domain)
		return -ENOMEM;

	ret = domain_prepare_identity_map(dev, domain, start, end);
	if (ret)
		domain_exit(domain);

	return ret;
}