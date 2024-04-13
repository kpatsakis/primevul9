static void mwifiex_cleanup_txq_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	struct sk_buff *skb;
	struct mwifiex_pcie_buf_desc *desc;
	struct mwifiex_pfu_buf_desc *desc2;
	int i;

	for (i = 0; i < MWIFIEX_MAX_TXRX_BD; i++) {
		if (reg->pfu_enabled) {
			desc2 = card->txbd_ring[i];
			if (card->tx_buf_list[i]) {
				skb = card->tx_buf_list[i];
				mwifiex_unmap_pci_memory(adapter, skb,
							 PCI_DMA_TODEVICE);
				dev_kfree_skb_any(skb);
			}
			memset(desc2, 0, sizeof(*desc2));
		} else {
			desc = card->txbd_ring[i];
			if (card->tx_buf_list[i]) {
				skb = card->tx_buf_list[i];
				mwifiex_unmap_pci_memory(adapter, skb,
							 PCI_DMA_TODEVICE);
				dev_kfree_skb_any(skb);
			}
			memset(desc, 0, sizeof(*desc));
		}
		card->tx_buf_list[i] = NULL;
	}

	atomic_set(&adapter->tx_hw_pending, 0);
	return;
}