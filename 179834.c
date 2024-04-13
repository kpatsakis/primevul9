static void mwifiex_pcie_card_reset_work(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;

	/* We can't afford to wait here; remove() might be waiting on us. If we
	 * can't grab the device lock, maybe we'll get another chance later.
	 */
	pci_try_reset_function(card->dev);
}