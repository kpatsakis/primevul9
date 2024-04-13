domain_context_mapping(struct dmar_domain *domain, struct device *dev)
{
	struct domain_context_mapping_data data;
	struct pasid_table *table;
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	table = intel_pasid_get_table(dev);

	if (!dev_is_pci(dev))
		return domain_context_mapping_one(domain, iommu, table,
						  bus, devfn);

	data.domain = domain;
	data.iommu = iommu;
	data.table = table;

	return pci_for_each_dma_alias(to_pci_dev(dev),
				      &domain_context_mapping_cb, &data);
}