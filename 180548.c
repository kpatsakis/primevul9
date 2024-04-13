static void domain_update_iotlb(struct dmar_domain *domain)
{
	struct device_domain_info *info;
	bool has_iotlb_device = false;

	assert_spin_locked(&device_domain_lock);

	list_for_each_entry(info, &domain->devices, link) {
		struct pci_dev *pdev;

		if (!info->dev || !dev_is_pci(info->dev))
			continue;

		pdev = to_pci_dev(info->dev);
		if (pdev->ats_enabled) {
			has_iotlb_device = true;
			break;
		}
	}

	domain->has_iotlb_device = has_iotlb_device;
}