static int domain_context_mapped_cb(struct pci_dev *pdev,
				    u16 alias, void *opaque)
{
	struct intel_iommu *iommu = opaque;

	return !device_context_mapped(iommu, PCI_BUS_NUM(alias), alias & 0xff);
}