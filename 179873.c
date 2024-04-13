static void mwifiex_pcie_reset_prepare(struct pci_dev *pdev)
{
	struct pcie_service_card *card = pci_get_drvdata(pdev);
	struct mwifiex_adapter *adapter = card->adapter;

	if (!adapter) {
		dev_err(&pdev->dev, "%s: adapter structure is not valid\n",
			__func__);
		return;
	}

	mwifiex_dbg(adapter, INFO,
		    "%s: vendor=0x%4.04x device=0x%4.04x rev=%d Pre-FLR\n",
		    __func__, pdev->vendor, pdev->device, pdev->revision);

	mwifiex_shutdown_sw(adapter);
	clear_bit(MWIFIEX_IFACE_WORK_DEVICE_DUMP, &card->work_flags);
	clear_bit(MWIFIEX_IFACE_WORK_CARD_RESET, &card->work_flags);
	mwifiex_dbg(adapter, INFO, "%s, successful\n", __func__);
}