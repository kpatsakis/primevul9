static int mwifiex_pcie_delete_sleep_cookie_buf(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card;

	if (!adapter)
		return 0;

	card = adapter->card;

	if (card && card->sleep_cookie_vbase) {
		pci_free_consistent(card->dev, sizeof(u32),
				    card->sleep_cookie_vbase,
				    card->sleep_cookie_pbase);
		card->sleep_cookie_vbase = NULL;
	}

	return 0;
}