static void mwifiex_pcie_reset_done(struct pci_dev *pdev)
{
	struct pcie_service_card *card = pci_get_drvdata(pdev);
	struct mwifiex_adapter *adapter = card->adapter;
	int ret;

	if (!adapter) {
		dev_err(&pdev->dev, "%s: adapter structure is not valid\n",
			__func__);
		return;
	}

	mwifiex_dbg(adapter, INFO,
		    "%s: vendor=0x%4.04x device=0x%4.04x rev=%d Post-FLR\n",
		    __func__, pdev->vendor, pdev->device, pdev->revision);

	ret = mwifiex_reinit_sw(adapter);
	if (ret)
		dev_err(&pdev->dev, "reinit failed: %d\n", ret);
	else
		mwifiex_dbg(adapter, INFO, "%s, successful\n", __func__);
}