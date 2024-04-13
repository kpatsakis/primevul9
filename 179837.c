static int mwifiex_pcie_delete_evtbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	mwifiex_cleanup_evt_ring(adapter);

	if (card->evtbd_ring_vbase)
		pci_free_consistent(card->dev, card->evtbd_ring_size,
				    card->evtbd_ring_vbase,
				    card->evtbd_ring_pbase);
	card->evtbd_wrptr = 0;
	card->evtbd_rdptr = 0 | reg->evt_rollover_ind;
	card->evtbd_ring_size = 0;
	card->evtbd_ring_vbase = NULL;
	card->evtbd_ring_pbase = 0;

	return 0;
}