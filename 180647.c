static void iommu_disable_dev_iotlb(struct device_domain_info *info)
{
	struct pci_dev *pdev;

	assert_spin_locked(&device_domain_lock);

	if (!dev_is_pci(info->dev))
		return;

	pdev = to_pci_dev(info->dev);

	if (info->ats_enabled) {
		pci_disable_ats(pdev);
		info->ats_enabled = 0;
		domain_update_iotlb(info->domain);
	}
#ifdef CONFIG_INTEL_IOMMU_SVM
	if (info->pri_enabled) {
		pci_disable_pri(pdev);
		info->pri_enabled = 0;
	}
	if (info->pasid_enabled) {
		pci_disable_pasid(pdev);
		info->pasid_enabled = 0;
	}
#endif
}