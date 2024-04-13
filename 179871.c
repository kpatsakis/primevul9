static void mwifiex_cleanup_rxq_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	struct mwifiex_pcie_buf_desc *desc;
	struct mwifiex_pfu_buf_desc *desc2;
	struct sk_buff *skb;
	int i;

	for (i = 0; i < MWIFIEX_MAX_TXRX_BD; i++) {
		if (reg->pfu_enabled) {
			desc2 = card->rxbd_ring[i];
			if (card->rx_buf_list[i]) {
				skb = card->rx_buf_list[i];
				mwifiex_unmap_pci_memory(adapter, skb,
							 PCI_DMA_FROMDEVICE);
				dev_kfree_skb_any(skb);
			}
			memset(desc2, 0, sizeof(*desc2));
		} else {
			desc = card->rxbd_ring[i];
			if (card->rx_buf_list[i]) {
				skb = card->rx_buf_list[i];
				mwifiex_unmap_pci_memory(adapter, skb,
							 PCI_DMA_FROMDEVICE);
				dev_kfree_skb_any(skb);
			}
			memset(desc, 0, sizeof(*desc));
		}
		card->rx_buf_list[i] = NULL;
	}

	return;
}