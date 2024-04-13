static void domain_context_clear(struct intel_iommu *iommu, struct device *dev)
{
	if (!iommu || !dev || !dev_is_pci(dev))
		return;

	pci_for_each_dma_alias(to_pci_dev(dev), &domain_context_clear_one_cb, iommu);
}