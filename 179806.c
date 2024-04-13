static void mwifiex_pcie_coredump(struct device *dev)
{
	struct pci_dev *pdev;
	struct pcie_service_card *card;

	pdev = container_of(dev, struct pci_dev, dev);
	card = pci_get_drvdata(pdev);

	if (!test_and_set_bit(MWIFIEX_IFACE_WORK_DEVICE_DUMP,
			      &card->work_flags))
		schedule_work(&card->work);
}