static int dwc3_pci_runtime_suspend(struct device *dev)
{
	struct dwc3_pci		*dwc = dev_get_drvdata(dev);

	if (device_can_wakeup(dev))
		return dwc3_pci_dsm(dwc, PCI_INTEL_BXT_STATE_D3);

	return -EBUSY;
}