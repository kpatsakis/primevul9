static struct dmar_domain *find_or_alloc_domain(struct device *dev, int gaw)
{
	struct device_domain_info *info;
	struct dmar_domain *domain = NULL;
	struct intel_iommu *iommu;
	u16 dma_alias;
	unsigned long flags;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return NULL;

	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		pci_for_each_dma_alias(pdev, get_last_alias, &dma_alias);

		spin_lock_irqsave(&device_domain_lock, flags);
		info = dmar_search_domain_by_dev_info(pci_domain_nr(pdev->bus),
						      PCI_BUS_NUM(dma_alias),
						      dma_alias & 0xff);
		if (info) {
			iommu = info->iommu;
			domain = info->domain;
		}
		spin_unlock_irqrestore(&device_domain_lock, flags);

		/* DMA alias already has a domain, use it */
		if (info)
			goto out;
	}

	/* Allocate and initialize new domain for the device */
	domain = alloc_domain(0);
	if (!domain)
		return NULL;
	if (domain_init(domain, iommu, gaw)) {
		domain_exit(domain);
		return NULL;
	}

out:

	return domain;
}