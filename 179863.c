static int mwifiex_init_txq_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	struct mwifiex_pcie_buf_desc *desc;
	struct mwifiex_pfu_buf_desc *desc2;
	int i;

	for (i = 0; i < MWIFIEX_MAX_TXRX_BD; i++) {
		card->tx_buf_list[i] = NULL;
		if (reg->pfu_enabled) {
			card->txbd_ring[i] = (void *)card->txbd_ring_vbase +
					     (sizeof(*desc2) * i);
			desc2 = card->txbd_ring[i];
			memset(desc2, 0, sizeof(*desc2));
		} else {
			card->txbd_ring[i] = (void *)card->txbd_ring_vbase +
					     (sizeof(*desc) * i);
			desc = card->txbd_ring[i];
			memset(desc, 0, sizeof(*desc));
		}
	}

	return 0;
}