static bool device_is_rmrr_locked(struct device *dev)
{
	if (!device_has_rmrr(dev))
		return false;

	if (dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(dev);

		if (IS_USB_DEVICE(pdev) || IS_GFX_DEVICE(pdev))
			return false;
	}

	return true;
}