static void mwifiex_pcie_down_dev(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	struct pci_dev *pdev = card->dev;

	if (mwifiex_write_reg(adapter, reg->drv_rdy, 0x00000000))
		mwifiex_dbg(adapter, ERROR, "Failed to write driver not-ready signature\n");

	pci_clear_master(pdev);

	adapter->seq_num = 0;

	mwifiex_pcie_free_buffers(adapter);
}