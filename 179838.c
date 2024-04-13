static bool mwifiex_pcie_ok_to_access_hw(struct mwifiex_adapter *adapter)
{
	u32 cookie_value;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (!reg->sleep_cookie)
		return true;

	if (card->sleep_cookie_vbase) {
		cookie_value = get_unaligned_le32(card->sleep_cookie_vbase);
		mwifiex_dbg(adapter, INFO,
			    "info: ACCESS_HW: sleep cookie=0x%x\n",
			    cookie_value);
		if (cookie_value == FW_AWAKE_COOKIE)
			return true;
	}

	return false;
}