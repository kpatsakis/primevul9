static void mwifiex_pcie_up_dev(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	struct pci_dev *pdev = card->dev;

	/* tx_buf_size might be changed to 3584 by firmware during
	 * data transfer, we should reset it to default size.
	 */
	adapter->tx_buf_size = card->pcie.tx_buf_size;

	mwifiex_pcie_alloc_buffers(adapter);

	pci_set_master(pdev);
}