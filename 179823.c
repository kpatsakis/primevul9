static void mwifiex_pcie_work(struct work_struct *work)
{
	struct pcie_service_card *card =
		container_of(work, struct pcie_service_card, work);

	if (test_and_clear_bit(MWIFIEX_IFACE_WORK_DEVICE_DUMP,
			       &card->work_flags))
		mwifiex_pcie_device_dump_work(card->adapter);
	if (test_and_clear_bit(MWIFIEX_IFACE_WORK_CARD_RESET,
			       &card->work_flags))
		mwifiex_pcie_card_reset_work(card->adapter);
}