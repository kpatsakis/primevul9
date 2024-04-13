static int mwifiex_pcie_create_txbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	/*
	 * driver maintaines the write pointer and firmware maintaines the read
	 * pointer. The write pointer starts at 0 (zero) while the read pointer
	 * starts at zero with rollover bit set
	 */
	card->txbd_wrptr = 0;

	if (reg->pfu_enabled)
		card->txbd_rdptr = 0;
	else
		card->txbd_rdptr |= reg->tx_rollover_ind;

	/* allocate shared memory for the BD ring and divide the same in to
	   several descriptors */
	if (reg->pfu_enabled)
		card->txbd_ring_size = sizeof(struct mwifiex_pfu_buf_desc) *
				       MWIFIEX_MAX_TXRX_BD;
	else
		card->txbd_ring_size = sizeof(struct mwifiex_pcie_buf_desc) *
				       MWIFIEX_MAX_TXRX_BD;

	mwifiex_dbg(adapter, INFO,
		    "info: txbd_ring: Allocating %d bytes\n",
		    card->txbd_ring_size);
	card->txbd_ring_vbase = pci_alloc_consistent(card->dev,
						     card->txbd_ring_size,
						     &card->txbd_ring_pbase);
	if (!card->txbd_ring_vbase) {
		mwifiex_dbg(adapter, ERROR,
			    "allocate consistent memory (%d bytes) failed!\n",
			    card->txbd_ring_size);
		return -ENOMEM;
	}
	mwifiex_dbg(adapter, DATA,
		    "info: txbd_ring - base: %p, pbase: %#x:%x, len: %x\n",
		    card->txbd_ring_vbase, (unsigned int)card->txbd_ring_pbase,
		    (u32)((u64)card->txbd_ring_pbase >> 32),
		    card->txbd_ring_size);

	return mwifiex_init_txq_ring(adapter);
}