mwifiex_pcie_send_data(struct mwifiex_adapter *adapter, struct sk_buff *skb,
		       struct mwifiex_tx_param *tx_param)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	u32 wrindx, num_tx_buffs, rx_val;
	int ret;
	dma_addr_t buf_pa;
	struct mwifiex_pcie_buf_desc *desc = NULL;
	struct mwifiex_pfu_buf_desc *desc2 = NULL;

	if (!(skb->data && skb->len)) {
		mwifiex_dbg(adapter, ERROR,
			    "%s(): invalid parameter <%p, %#x>\n",
			    __func__, skb->data, skb->len);
		return -1;
	}

	if (!mwifiex_pcie_ok_to_access_hw(adapter))
		mwifiex_pm_wakeup_card(adapter);

	num_tx_buffs = MWIFIEX_MAX_TXRX_BD << reg->tx_start_ptr;
	mwifiex_dbg(adapter, DATA,
		    "info: SEND DATA: <Rd: %#x, Wr: %#x>\n",
		card->txbd_rdptr, card->txbd_wrptr);
	if (mwifiex_pcie_txbd_not_full(card)) {
		u8 *payload;

		adapter->data_sent = true;
		payload = skb->data;
		put_unaligned_le16((u16)skb->len, payload + 0);
		put_unaligned_le16(MWIFIEX_TYPE_DATA, payload + 2);

		if (mwifiex_map_pci_memory(adapter, skb, skb->len,
					   PCI_DMA_TODEVICE))
			return -1;

		wrindx = (card->txbd_wrptr & reg->tx_mask) >> reg->tx_start_ptr;
		buf_pa = MWIFIEX_SKB_DMA_ADDR(skb);
		card->tx_buf_list[wrindx] = skb;
		atomic_inc(&adapter->tx_hw_pending);

		if (reg->pfu_enabled) {
			desc2 = card->txbd_ring[wrindx];
			desc2->paddr = buf_pa;
			desc2->len = (u16)skb->len;
			desc2->frag_len = (u16)skb->len;
			desc2->offset = 0;
			desc2->flags = MWIFIEX_BD_FLAG_FIRST_DESC |
					 MWIFIEX_BD_FLAG_LAST_DESC;
		} else {
			desc = card->txbd_ring[wrindx];
			desc->paddr = buf_pa;
			desc->len = (u16)skb->len;
			desc->flags = MWIFIEX_BD_FLAG_FIRST_DESC |
				      MWIFIEX_BD_FLAG_LAST_DESC;
		}

		switch (card->dev->device) {
		case PCIE_DEVICE_ID_MARVELL_88W8766P:
			card->txbd_wrptr++;
			break;
		case PCIE_DEVICE_ID_MARVELL_88W8897:
		case PCIE_DEVICE_ID_MARVELL_88W8997:
			card->txbd_wrptr += reg->ring_tx_start_ptr;
			break;
		}

		if ((card->txbd_wrptr & reg->tx_mask) == num_tx_buffs)
			card->txbd_wrptr = ((card->txbd_wrptr &
						reg->tx_rollover_ind) ^
						reg->tx_rollover_ind);

		rx_val = card->rxbd_rdptr & reg->rx_wrap_mask;
		/* Write the TX ring write pointer in to reg->tx_wrptr */
		if (mwifiex_write_reg(adapter, reg->tx_wrptr,
				      card->txbd_wrptr | rx_val)) {
			mwifiex_dbg(adapter, ERROR,
				    "SEND DATA: failed to write reg->tx_wrptr\n");
			ret = -1;
			goto done_unmap;
		}
		if ((mwifiex_pcie_txbd_not_full(card)) &&
		    tx_param->next_pkt_len) {
			/* have more packets and TxBD still can hold more */
			mwifiex_dbg(adapter, DATA,
				    "SEND DATA: delay dnld-rdy interrupt.\n");
			adapter->data_sent = false;
		} else {
			/* Send the TX ready interrupt */
			if (mwifiex_write_reg(adapter, PCIE_CPU_INT_EVENT,
					      CPU_INTR_DNLD_RDY)) {
				mwifiex_dbg(adapter, ERROR,
					    "SEND DATA: failed to assert dnld-rdy interrupt.\n");
				ret = -1;
				goto done_unmap;
			}
		}
		mwifiex_dbg(adapter, DATA,
			    "info: SEND DATA: Updated <Rd: %#x, Wr:\t"
			    "%#x> and sent packet to firmware successfully\n",
			    card->txbd_rdptr, card->txbd_wrptr);
	} else {
		mwifiex_dbg(adapter, DATA,
			    "info: TX Ring full, can't send packets to fw\n");
		adapter->data_sent = true;
		/* Send the TX ready interrupt */
		if (mwifiex_write_reg(adapter, PCIE_CPU_INT_EVENT,
				      CPU_INTR_DNLD_RDY))
			mwifiex_dbg(adapter, ERROR,
				    "SEND DATA: failed to assert door-bell intr\n");
		return -EBUSY;
	}

	return -EINPROGRESS;
done_unmap:
	mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);
	card->tx_buf_list[wrindx] = NULL;
	atomic_dec(&adapter->tx_hw_pending);
	if (reg->pfu_enabled)
		memset(desc2, 0, sizeof(*desc2));
	else
		memset(desc, 0, sizeof(*desc));

	return ret;
}