static int mwifiex_pcie_delete_txbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	mwifiex_cleanup_txq_ring(adapter);

	if (card->txbd_ring_vbase)
		pci_free_consistent(card->dev, card->txbd_ring_size,
				    card->txbd_ring_vbase,
				    card->txbd_ring_pbase);
	card->txbd_ring_size = 0;
	card->txbd_wrptr = 0;
	card->txbd_rdptr = 0 | reg->tx_rollover_ind;
	card->txbd_ring_vbase = NULL;
	card->txbd_ring_pbase = 0;

	return 0;
}