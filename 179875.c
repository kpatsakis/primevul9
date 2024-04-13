mwifiex_check_winner_status(struct mwifiex_adapter *adapter)
{
	u32 winner = 0;
	int ret = 0;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (mwifiex_read_reg(adapter, reg->fw_status, &winner)) {
		ret = -1;
	} else if (!winner) {
		mwifiex_dbg(adapter, INFO, "PCI-E is the winner\n");
		adapter->winner = 1;
	} else {
		mwifiex_dbg(adapter, ERROR,
			    "PCI-E is not the winner <%#x>", winner);
	}

	return ret;
}