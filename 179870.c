static int mwifiex_pcie_process_recv_data(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	u32 wrptr, rd_index, tx_val;
	dma_addr_t buf_pa;
	int ret = 0;
	struct sk_buff *skb_tmp = NULL;
	struct mwifiex_pcie_buf_desc *desc;
	struct mwifiex_pfu_buf_desc *desc2;

	if (!mwifiex_pcie_ok_to_access_hw(adapter))
		mwifiex_pm_wakeup_card(adapter);

	/* Read the RX ring Write pointer set by firmware */
	if (mwifiex_read_reg(adapter, reg->rx_wrptr, &wrptr)) {
		mwifiex_dbg(adapter, ERROR,
			    "RECV DATA: failed to read reg->rx_wrptr\n");
		ret = -1;
		goto done;
	}
	card->rxbd_wrptr = wrptr;

	while (((wrptr & reg->rx_mask) !=
		(card->rxbd_rdptr & reg->rx_mask)) ||
	       ((wrptr & reg->rx_rollover_ind) ==
		(card->rxbd_rdptr & reg->rx_rollover_ind))) {
		struct sk_buff *skb_data;
		u16 rx_len;

		rd_index = card->rxbd_rdptr & reg->rx_mask;
		skb_data = card->rx_buf_list[rd_index];

		/* If skb allocation was failed earlier for Rx packet,
		 * rx_buf_list[rd_index] would have been left with a NULL.
		 */
		if (!skb_data)
			return -ENOMEM;

		mwifiex_unmap_pci_memory(adapter, skb_data, PCI_DMA_FROMDEVICE);
		card->rx_buf_list[rd_index] = NULL;

		/* Get data length from interface header -
		 * first 2 bytes for len, next 2 bytes is for type
		 */
		rx_len = get_unaligned_le16(skb_data->data);
		if (WARN_ON(rx_len <= adapter->intf_hdr_len ||
			    rx_len > MWIFIEX_RX_DATA_BUF_SIZE)) {
			mwifiex_dbg(adapter, ERROR,
				    "Invalid RX len %d, Rd=%#x, Wr=%#x\n",
				    rx_len, card->rxbd_rdptr, wrptr);
			dev_kfree_skb_any(skb_data);
		} else {
			skb_put(skb_data, rx_len);
			mwifiex_dbg(adapter, DATA,
				    "info: RECV DATA: Rd=%#x, Wr=%#x, Len=%d\n",
				    card->rxbd_rdptr, wrptr, rx_len);
			skb_pull(skb_data, adapter->intf_hdr_len);
			if (adapter->rx_work_enabled) {
				skb_queue_tail(&adapter->rx_data_q, skb_data);
				adapter->data_received = true;
				atomic_inc(&adapter->rx_pending);
			} else {
				mwifiex_handle_rx_packet(adapter, skb_data);
			}
		}

		skb_tmp = mwifiex_alloc_dma_align_buf(MWIFIEX_RX_DATA_BUF_SIZE,
						      GFP_KERNEL);
		if (!skb_tmp) {
			mwifiex_dbg(adapter, ERROR,
				    "Unable to allocate skb.\n");
			return -ENOMEM;
		}

		if (mwifiex_map_pci_memory(adapter, skb_tmp,
					   MWIFIEX_RX_DATA_BUF_SIZE,
					   PCI_DMA_FROMDEVICE))
			return -1;

		buf_pa = MWIFIEX_SKB_DMA_ADDR(skb_tmp);

		mwifiex_dbg(adapter, INFO,
			    "RECV DATA: Attach new sk_buff %p at rxbd_rdidx=%d\n",
			    skb_tmp, rd_index);
		card->rx_buf_list[rd_index] = skb_tmp;

		if (reg->pfu_enabled) {
			desc2 = card->rxbd_ring[rd_index];
			desc2->paddr = buf_pa;
			desc2->len = skb_tmp->len;
			desc2->frag_len = skb_tmp->len;
			desc2->offset = 0;
			desc2->flags = reg->ring_flag_sop | reg->ring_flag_eop;
		} else {
			desc = card->rxbd_ring[rd_index];
			desc->paddr = buf_pa;
			desc->len = skb_tmp->len;
			desc->flags = 0;
		}

		if ((++card->rxbd_rdptr & reg->rx_mask) ==
							MWIFIEX_MAX_TXRX_BD) {
			card->rxbd_rdptr = ((card->rxbd_rdptr &
					     reg->rx_rollover_ind) ^
					     reg->rx_rollover_ind);
		}
		mwifiex_dbg(adapter, DATA,
			    "info: RECV DATA: <Rd: %#x, Wr: %#x>\n",
			    card->rxbd_rdptr, wrptr);

		tx_val = card->txbd_wrptr & reg->tx_wrap_mask;
		/* Write the RX ring read pointer in to reg->rx_rdptr */
		if (mwifiex_write_reg(adapter, reg->rx_rdptr,
				      card->rxbd_rdptr | tx_val)) {
			mwifiex_dbg(adapter, DATA,
				    "RECV DATA: failed to write reg->rx_rdptr\n");
			ret = -1;
			goto done;
		}

		/* Read the RX ring Write pointer set by firmware */
		if (mwifiex_read_reg(adapter, reg->rx_wrptr, &wrptr)) {
			mwifiex_dbg(adapter, ERROR,
				    "RECV DATA: failed to read reg->rx_wrptr\n");
			ret = -1;
			goto done;
		}
		mwifiex_dbg(adapter, DATA,
			    "info: RECV DATA: Rcvd packet from fw successfully\n");
		card->rxbd_wrptr = wrptr;
	}

done:
	return ret;
}