static int mwifiex_pcie_create_evtbd_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	/*
	 * driver maintaines the read pointer and firmware maintaines the write
	 * pointer. The write pointer starts at 0 (zero) while the read pointer
	 * starts at zero with rollover bit set
	 */
	card->evtbd_wrptr = 0;
	card->evtbd_rdptr = reg->evt_rollover_ind;

	card->evtbd_ring_size = sizeof(struct mwifiex_evt_buf_desc) *
				MWIFIEX_MAX_EVT_BD;

	mwifiex_dbg(adapter, INFO,
		    "info: evtbd_ring: Allocating %d bytes\n",
		card->evtbd_ring_size);
	card->evtbd_ring_vbase = pci_alloc_consistent(card->dev,
						      card->evtbd_ring_size,
						      &card->evtbd_ring_pbase);
	if (!card->evtbd_ring_vbase) {
		mwifiex_dbg(adapter, ERROR,
			    "allocate consistent memory (%d bytes) failed!\n",
			    card->evtbd_ring_size);
		return -ENOMEM;
	}

	mwifiex_dbg(adapter, EVENT,
		    "info: CMDRSP/EVT bd_ring - base: %p pbase: %#x:%x len: %#x\n",
		    card->evtbd_ring_vbase, (u32)card->evtbd_ring_pbase,
		    (u32)((u64)card->evtbd_ring_pbase >> 32),
		    card->evtbd_ring_size);

	return mwifiex_pcie_init_evt_ring(adapter);
}