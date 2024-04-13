static int mwifiex_read_reg(struct mwifiex_adapter *adapter, int reg, u32 *data)
{
	struct pcie_service_card *card = adapter->card;

	*data = ioread32(card->pci_mmap1 + reg);
	if (*data == 0xffffffff)
		return 0xffffffff;

	return 0;
}