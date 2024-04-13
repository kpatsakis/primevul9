static void mwifiex_pcie_remove(struct pci_dev *pdev)
{
	struct pcie_service_card *card;
	struct mwifiex_adapter *adapter;
	struct mwifiex_private *priv;
	const struct mwifiex_pcie_card_reg *reg;
	u32 fw_status;
	int ret;

	card = pci_get_drvdata(pdev);

	wait_for_completion(&card->fw_done);

	adapter = card->adapter;
	if (!adapter || !adapter->priv_num)
		return;

	reg = card->pcie.reg;
	if (reg)
		ret = mwifiex_read_reg(adapter, reg->fw_status, &fw_status);
	else
		fw_status = -1;

	if (fw_status == FIRMWARE_READY_PCIE && !adapter->mfg_mode) {
		mwifiex_deauthenticate_all(adapter);

		priv = mwifiex_get_priv(adapter, MWIFIEX_BSS_ROLE_ANY);

		mwifiex_disable_auto_ds(priv);

		mwifiex_init_shutdown_fw(priv, MWIFIEX_FUNC_SHUTDOWN);
	}

	mwifiex_remove_card(adapter);
}