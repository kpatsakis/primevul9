static int mwifiex_read_reg_byte(struct mwifiex_adapter *adapter,
				 int reg, u8 *data)
{
	struct pcie_service_card *card = adapter->card;

	*data = ioread8(card->pci_mmap1 + reg);

	return 0;
}