mwifiex_map_pci_memory(struct mwifiex_adapter *adapter, struct sk_buff *skb,
		       size_t size, int flags)
{
	struct pcie_service_card *card = adapter->card;
	struct mwifiex_dma_mapping mapping;

	mapping.addr = pci_map_single(card->dev, skb->data, size, flags);
	if (pci_dma_mapping_error(card->dev, mapping.addr)) {
		mwifiex_dbg(adapter, ERROR, "failed to map pci memory!\n");
		return -1;
	}
	mapping.len = size;
	mwifiex_store_mapping(skb, &mapping);
	return 0;
}