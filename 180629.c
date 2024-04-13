static inline void iommu_prepare_isa(void)
{
	struct pci_dev *pdev;
	int ret;

	pdev = pci_get_class(PCI_CLASS_BRIDGE_ISA << 8, NULL);
	if (!pdev)
		return;

	pr_info("Prepare 0-16MiB unity mapping for LPC\n");
	ret = iommu_prepare_identity_map(&pdev->dev, 0, 16*1024*1024 - 1);

	if (ret)
		pr_err("Failed to create 0-16MiB identity map - floppy might not work\n");

	pci_dev_put(pdev);
}