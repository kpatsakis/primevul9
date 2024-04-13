static int domain_context_clear_one_cb(struct pci_dev *pdev, u16 alias, void *opaque)
{
	struct intel_iommu *iommu = opaque;

	domain_context_clear_one(iommu, PCI_BUS_NUM(alias), alias & 0xff);
	return 0;
}