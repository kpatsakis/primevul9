static int mwifiex_pcie_cmdrsp_complete(struct mwifiex_adapter *adapter,
					struct sk_buff *skb)
{
	struct pcie_service_card *card = adapter->card;

	if (skb) {
		card->cmdrsp_buf = skb;
		skb_push(card->cmdrsp_buf, adapter->intf_hdr_len);
		if (mwifiex_map_pci_memory(adapter, skb, MWIFIEX_UPLD_SIZE,
					   PCI_DMA_FROMDEVICE))
			return -1;
	}

	return 0;
}