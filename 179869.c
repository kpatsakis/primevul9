static void mwifiex_pcie_device_dump(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;

	if (!test_and_set_bit(MWIFIEX_IFACE_WORK_DEVICE_DUMP,
			      &card->work_flags))
		schedule_work(&card->work);
}