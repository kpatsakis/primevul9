static void mwifiex_cleanup_evt_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	struct mwifiex_evt_buf_desc *desc;
	struct sk_buff *skb;
	int i;

	for (i = 0; i < MWIFIEX_MAX_EVT_BD; i++) {
		desc = card->evtbd_ring[i];
		if (card->evt_buf_list[i]) {
			skb = card->evt_buf_list[i];
			mwifiex_unmap_pci_memory(adapter, skb,
						 PCI_DMA_FROMDEVICE);
			dev_kfree_skb_any(skb);
		}
		card->evt_buf_list[i] = NULL;
		memset(desc, 0, sizeof(*desc));
	}

	return;
}