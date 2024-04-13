static int dwc3_pci_resume(struct device *dev)
{
	struct dwc3_pci		*dwc = dev_get_drvdata(dev);

	return dwc3_pci_dsm(dwc, PCI_INTEL_BXT_STATE_D0);
}