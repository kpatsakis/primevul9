static int domain_context_mapped(struct device *dev)
{
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	if (!dev_is_pci(dev))
		return device_context_mapped(iommu, bus, devfn);

	return !pci_for_each_dma_alias(to_pci_dev(dev),
				       domain_context_mapped_cb, iommu);
}