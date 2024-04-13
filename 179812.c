static int mwifiex_pcie_send_data_complete(struct mwifiex_adapter *adapter)
{
	struct sk_buff *skb;
	u32 wrdoneidx, rdptr, num_tx_buffs, unmap_count = 0;
	struct mwifiex_pcie_buf_desc *desc;
	struct mwifiex_pfu_buf_desc *desc2;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (!mwifiex_pcie_ok_to_access_hw(adapter))
		mwifiex_pm_wakeup_card(adapter);

	/* Read the TX ring read pointer set by firmware */
	if (mwifiex_read_reg(adapter, reg->tx_rdptr, &rdptr)) {
		mwifiex_dbg(adapter, ERROR,
			    "SEND COMP: failed to read reg->tx_rdptr\n");
		return -1;
	}

	mwifiex_dbg(adapter, DATA,
		    "SEND COMP: rdptr_prev=0x%x, rdptr=0x%x\n",
		    card->txbd_rdptr, rdptr);

	num_tx_buffs = MWIFIEX_MAX_TXRX_BD << reg->tx_start_ptr;
	/* free from previous txbd_rdptr to current txbd_rdptr */
	while (((card->txbd_rdptr & reg->tx_mask) !=
		(rdptr & reg->tx_mask)) ||
	       ((card->txbd_rdptr & reg->tx_rollover_ind) !=
		(rdptr & reg->tx_rollover_ind))) {
		wrdoneidx = (card->txbd_rdptr & reg->tx_mask) >>
			    reg->tx_start_ptr;

		skb = card->tx_buf_list[wrdoneidx];

		if (skb) {
			mwifiex_dbg(adapter, DATA,
				    "SEND COMP: Detach skb %p at txbd_rdidx=%d\n",
				    skb, wrdoneidx);
			mwifiex_unmap_pci_memory(adapter, skb,
						 PCI_DMA_TODEVICE);

			unmap_count++;

			if (card->txbd_flush)
				mwifiex_write_data_complete(adapter, skb, 0,
							    -1);
			else
				mwifiex_write_data_complete(adapter, skb, 0, 0);
			atomic_dec(&adapter->tx_hw_pending);
		}

		card->tx_buf_list[wrdoneidx] = NULL;

		if (reg->pfu_enabled) {
			desc2 = card->txbd_ring[wrdoneidx];
			memset(desc2, 0, sizeof(*desc2));
		} else {
			desc = card->txbd_ring[wrdoneidx];
			memset(desc, 0, sizeof(*desc));
		}
		switch (card->dev->device) {
		case PCIE_DEVICE_ID_MARVELL_88W8766P:
			card->txbd_rdptr++;
			break;
		case PCIE_DEVICE_ID_MARVELL_88W8897:
		case PCIE_DEVICE_ID_MARVELL_88W8997:
			card->txbd_rdptr += reg->ring_tx_start_ptr;
			break;
		}


		if ((card->txbd_rdptr & reg->tx_mask) == num_tx_buffs)
			card->txbd_rdptr = ((card->txbd_rdptr &
					     reg->tx_rollover_ind) ^
					     reg->tx_rollover_ind);
	}

	if (unmap_count)
		adapter->data_sent = false;

	if (card->txbd_flush) {
		if (mwifiex_pcie_txbd_empty(card, card->txbd_rdptr))
			card->txbd_flush = 0;
		else
			mwifiex_clean_pcie_ring_buf(adapter);
	}

	return 0;
}