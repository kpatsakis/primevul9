static void mwifiex_pcie_card_reset(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;

	if (!test_and_set_bit(MWIFIEX_IFACE_WORK_CARD_RESET, &card->work_flags))
		schedule_work(&card->work);
}