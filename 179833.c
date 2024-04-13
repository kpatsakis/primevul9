static int mwifiex_pcie_create_rxbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	/*
	 * driver maintaines the read pointer and firmware maintaines the write
	 * pointer. The write pointer starts at 0 (zero) while the read pointer
	 * starts at zero with rollover bit set
	 */
	card->rxbd_wrptr = 0;
	card->rxbd_rdptr = reg->rx_rollover_ind;

	if (reg->pfu_enabled)
		card->rxbd_ring_size = sizeof(struct mwifiex_pfu_buf_desc) *
				       MWIFIEX_MAX_TXRX_BD;
	else
		card->rxbd_ring_size = sizeof(struct mwifiex_pcie_buf_desc) *
				       MWIFIEX_MAX_TXRX_BD;

	mwifiex_dbg(adapter, INFO,
		    "info: rxbd_ring: Allocating %d bytes\n",
		    card->rxbd_ring_size);
	card->rxbd_ring_vbase = pci_alloc_consistent(card->dev,
						     card->rxbd_ring_size,
						     &card->rxbd_ring_pbase);
	if (!card->rxbd_ring_vbase) {
		mwifiex_dbg(adapter, ERROR,
			    "allocate consistent memory (%d bytes) failed!\n",
			    card->rxbd_ring_size);
		return -ENOMEM;
	}

	mwifiex_dbg(adapter, DATA,
		    "info: rxbd_ring - base: %p, pbase: %#x:%x, len: %#x\n",
		    card->rxbd_ring_vbase, (u32)card->rxbd_ring_pbase,
		    (u32)((u64)card->rxbd_ring_pbase >> 32),
		    card->rxbd_ring_size);

	return mwifiex_init_rxq_ring(adapter);
}