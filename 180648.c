static struct dmar_domain *get_domain_for_dev(struct device *dev, int gaw)
{
	struct dmar_domain *domain, *tmp;

	domain = find_domain(dev);
	if (domain)
		goto out;

	domain = find_or_alloc_domain(dev, gaw);
	if (!domain)
		goto out;

	tmp = set_domain_for_dev(dev, domain);
	if (!tmp || domain != tmp) {
		domain_exit(domain);
		domain = tmp;
	}

out:

	return domain;
}