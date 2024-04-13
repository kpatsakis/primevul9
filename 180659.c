static int iommu_should_identity_map(struct device *dev, int startup)
{

	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		if (device_is_rmrr_locked(dev))
			return 0;

		/*
		 * Prevent any device marked as untrusted from getting
		 * placed into the statically identity mapping domain.
		 */
		if (pdev->untrusted)
			return 0;

		if ((iommu_identity_mapping & IDENTMAP_AZALIA) && IS_AZALIA(pdev))
			return 1;

		if ((iommu_identity_mapping & IDENTMAP_GFX) && IS_GFX_DEVICE(pdev))
			return 1;

		if (!(iommu_identity_mapping & IDENTMAP_ALL))
			return 0;

		/*
		 * We want to start off with all devices in the 1:1 domain, and
		 * take them out later if we find they can't access all of memory.
		 *
		 * However, we can't do this for PCI devices behind bridges,
		 * because all PCI devices behind the same bridge will end up
		 * with the same source-id on their transactions.
		 *
		 * Practically speaking, we can't change things around for these
		 * devices at run-time, because we can't be sure there'll be no
		 * DMA transactions in flight for any of their siblings.
		 *
		 * So PCI devices (unless they're on the root bus) as well as
		 * their parent PCI-PCI or PCIe-PCI bridges must be left _out_ of
		 * the 1:1 domain, just in _case_ one of their siblings turns out
		 * not to be able to map all of memory.
		 */
		if (!pci_is_pcie(pdev)) {
			if (!pci_is_root_bus(pdev->bus))
				return 0;
			if (pdev->class >> 8 == PCI_CLASS_BRIDGE_PCI)
				return 0;
		} else if (pci_pcie_type(pdev) == PCI_EXP_TYPE_PCI_BRIDGE)
			return 0;
	} else {
		if (device_has_rmrr(dev))
			return 0;
	}

	/*
	 * At boot time, we don't yet know if devices will be 64-bit capable.
	 * Assume that they will — if they turn out not to be, then we can
	 * take them out of the 1:1 domain later.
	 */
	if (!startup) {
		/*
		 * If the device's dma_mask is less than the system's memory
		 * size then this is not a candidate for identity mapping.
		 */
		u64 dma_mask = *dev->dma_mask;

		if (dev->coherent_dma_mask &&
		    dev->coherent_dma_mask < dma_mask)
			dma_mask = dev->coherent_dma_mask;

		return dma_mask >= dma_get_required_mask(dev);
	}

	return 1;
}