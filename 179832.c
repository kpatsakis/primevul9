static void mwifiex_cleanup_pcie(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	struct pci_dev *pdev = card->dev;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	int ret;
	u32 fw_status;

	cancel_work_sync(&card->work);

	ret = mwifiex_read_reg(adapter, reg->fw_status, &fw_status);
	if (fw_status == FIRMWARE_READY_PCIE) {
		mwifiex_dbg(adapter, INFO,
			    "Clearing driver ready signature\n");
		if (mwifiex_write_reg(adapter, reg->drv_rdy, 0x00000000))
			mwifiex_dbg(adapter, ERROR,
				    "Failed to write driver not-ready signature\n");
	}

	pci_disable_device(pdev);

	pci_iounmap(pdev, card->pci_mmap);
	pci_iounmap(pdev, card->pci_mmap1);
	pci_release_region(pdev, 2);
	pci_release_region(pdev, 0);

	mwifiex_pcie_free_buffers(adapter);
}