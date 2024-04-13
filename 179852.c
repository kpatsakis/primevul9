static void mwifiex_pcie_free_buffers(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (reg->sleep_cookie)
		mwifiex_pcie_delete_sleep_cookie_buf(adapter);

	mwifiex_pcie_delete_cmdrsp_buf(adapter);
	mwifiex_pcie_delete_evtbd_ring(adapter);
	mwifiex_pcie_delete_rxbd_ring(adapter);
	mwifiex_pcie_delete_txbd_ring(adapter);
}