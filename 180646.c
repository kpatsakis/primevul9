domain_context_mapping(struct dmar_domain *domain, struct device *dev)
{
	struct intel_iommu *iommu;
	u8 bus, devfn;
	struct domain_context_mapping_data data;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	if (!dev_is_pci(dev))
		return domain_context_mapping_one(domain, iommu, bus, devfn);

	data.domain = domain;
	data.iommu = iommu;

	return pci_for_each_dma_alias(to_pci_dev(dev),
				      &domain_context_mapping_cb, &data);
}