static struct dmar_domain *set_domain_for_dev(struct device *dev,
					      struct dmar_domain *domain)
{
	struct intel_iommu *iommu;
	struct dmar_domain *tmp;
	u16 req_id, dma_alias;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return NULL;

	req_id = ((u16)bus << 8) | devfn;

	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		pci_for_each_dma_alias(pdev, get_last_alias, &dma_alias);

		/* register PCI DMA alias device */
		if (req_id != dma_alias) {
			tmp = dmar_insert_one_dev_info(iommu, PCI_BUS_NUM(dma_alias),
					dma_alias & 0xff, NULL, domain);

			if (!tmp || tmp != domain)
				return tmp;
		}
	}

	tmp = dmar_insert_one_dev_info(iommu, bus, devfn, dev, domain);
	if (!tmp || tmp != domain)
		return tmp;

	return domain;
}