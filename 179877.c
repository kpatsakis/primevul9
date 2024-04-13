static int mwifiex_write_reg(struct mwifiex_adapter *adapter, int reg, u32 data)
{
	struct pcie_service_card *card = adapter->card;

	iowrite32(data, card->pci_mmap1 + reg);

	return 0;
}