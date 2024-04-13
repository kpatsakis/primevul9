static void mwifiex_unmap_pci_memory(struct mwifiex_adapter *adapter,
				     struct sk_buff *skb, int flags)
{
	struct pcie_service_card *card = adapter->card;
	struct mwifiex_dma_mapping mapping;

	mwifiex_get_mapping(skb, &mapping);
	pci_unmap_single(card->dev, mapping.addr, mapping.len, flags);
}