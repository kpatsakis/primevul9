static int __init iommu_prepare_static_identity_mapping(int hw)
{
	struct pci_dev *pdev = NULL;
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;
	struct device *dev;
	int i;
	int ret = 0;

	for_each_pci_dev(pdev) {
		ret = dev_prepare_static_identity_mapping(&pdev->dev, hw);
		if (ret)
			return ret;
	}

	for_each_active_iommu(iommu, drhd)
		for_each_active_dev_scope(drhd->devices, drhd->devices_cnt, i, dev) {
			struct acpi_device_physical_node *pn;
			struct acpi_device *adev;

			if (dev->bus != &acpi_bus_type)
				continue;

			adev= to_acpi_device(dev);
			mutex_lock(&adev->physical_node_lock);
			list_for_each_entry(pn, &adev->physical_node_list, node) {
				ret = dev_prepare_static_identity_mapping(pn->dev, hw);
				if (ret)
					break;
			}
			mutex_unlock(&adev->physical_node_lock);
			if (ret)
				return ret;
		}

	return 0;
}