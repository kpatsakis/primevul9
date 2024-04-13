static int device_notifier(struct notifier_block *nb,
				  unsigned long action, void *data)
{
	struct device *dev = data;
	struct dmar_domain *domain;

	if (iommu_dummy(dev))
		return 0;

	if (action != BUS_NOTIFY_REMOVED_DEVICE)
		return 0;

	domain = find_domain(dev);
	if (!domain)
		return 0;

	dmar_remove_one_dev_info(domain, dev);
	if (!domain_type_is_vm_or_si(domain) && list_empty(&domain->devices))
		domain_exit(domain);

	return 0;
}