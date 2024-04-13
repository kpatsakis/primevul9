static int dwc3_pci_runtime_resume(struct device *dev)
{
	struct dwc3_pci		*dwc = dev_get_drvdata(dev);
	int			ret;

	ret = dwc3_pci_dsm(dwc, PCI_INTEL_BXT_STATE_D0);
	if (ret)
		return ret;

	queue_work(pm_wq, &dwc->wakeup_work);

	return 0;
}