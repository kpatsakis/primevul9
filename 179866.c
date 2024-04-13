static int mwifiex_pcie_init_fw_port(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	int tx_wrap = card->txbd_wrptr & reg->tx_wrap_mask;

	/* Write the RX ring read pointer in to reg->rx_rdptr */
	if (mwifiex_write_reg(adapter, reg->rx_rdptr, card->rxbd_rdptr |
			      tx_wrap)) {
		mwifiex_dbg(adapter, ERROR,
			    "RECV DATA: failed to write reg->rx_rdptr\n");
		return -1;
	}
	return 0;
}