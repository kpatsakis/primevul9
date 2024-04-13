static int mwifiex_pm_wakeup_card(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	mwifiex_dbg(adapter, EVENT,
		    "event: Wakeup device...\n");

	if (reg->sleep_cookie)
		mwifiex_pcie_dev_wakeup_delay(adapter);

	/* Accessing fw_status register will wakeup device */
	if (mwifiex_write_reg(adapter, reg->fw_status, FIRMWARE_READY_PCIE)) {
		mwifiex_dbg(adapter, ERROR,
			    "Writing fw_status register failed\n");
		return -1;
	}

	if (reg->sleep_cookie) {
		mwifiex_pcie_dev_wakeup_delay(adapter);
		mwifiex_dbg(adapter, INFO,
			    "PCIE wakeup: Setting PS_STATE_AWAKE\n");
		adapter->ps_state = PS_STATE_AWAKE;
	}

	return 0;
}