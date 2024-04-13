static void dwc3_pci_remove(struct pci_dev *pci)
{
	struct dwc3_pci		*dwc = pci_get_drvdata(pci);
	struct pci_dev		*pdev = dwc->pci;

	if (pdev->device == PCI_DEVICE_ID_INTEL_BYT)
		gpiod_remove_lookup_table(&platform_bytcr_gpios);
#ifdef CONFIG_PM
	cancel_work_sync(&dwc->wakeup_work);
#endif
	device_init_wakeup(&pci->dev, false);
	pm_runtime_get(&pci->dev);
	platform_device_unregister(dwc->dwc3);
}