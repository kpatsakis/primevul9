static int mwifiex_pcie_delete_cmdrsp_buf(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card;

	if (!adapter)
		return 0;

	card = adapter->card;

	if (card && card->cmdrsp_buf) {
		mwifiex_unmap_pci_memory(adapter, card->cmdrsp_buf,
					 PCI_DMA_FROMDEVICE);
		dev_kfree_skb_any(card->cmdrsp_buf);
		card->cmdrsp_buf = NULL;
	}

	if (card && card->cmd_buf) {
		mwifiex_unmap_pci_memory(adapter, card->cmd_buf,
					 PCI_DMA_TODEVICE);
		dev_kfree_skb_any(card->cmd_buf);
		card->cmd_buf = NULL;
	}
	return 0;
}