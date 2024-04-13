static int mwifiex_pcie_delete_rxbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	mwifiex_cleanup_rxq_ring(adapter);

	if (card->rxbd_ring_vbase)
		pci_free_consistent(card->dev, card->rxbd_ring_size,
				    card->rxbd_ring_vbase,
				    card->rxbd_ring_pbase);
	card->rxbd_ring_size = 0;
	card->rxbd_wrptr = 0;
	card->rxbd_rdptr = 0 | reg->rx_rollover_ind;
	card->rxbd_ring_vbase = NULL;
	card->rxbd_ring_pbase = 0;

	return 0;
}