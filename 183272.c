static int device_notifier(struct notifier_block *nb,
				  unsigned long action, void *data)
{
	struct device *dev = data;
	struct dmar_domain *domain;

	if (iommu_dummy(dev))
		return 0;

	if (action == BUS_NOTIFY_REMOVED_DEVICE) {
		domain = find_domain(dev);
		if (!domain)
			return 0;

		dmar_remove_one_dev_info(dev);
		if (!domain_type_is_vm_or_si(domain) &&
		    list_empty(&domain->devices))
			domain_exit(domain);
	} else if (action == BUS_NOTIFY_ADD_DEVICE) {
		if (iommu_should_identity_map(dev, 1))
			domain_add_dev_info(si_domain, dev);
	}

	return 0;
}