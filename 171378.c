 */
static int ql_send_map(struct ql3_adapter *qdev,
				struct ob_mac_iocb_req *mac_iocb_ptr,
				struct ql_tx_buf_cb *tx_cb,
				struct sk_buff *skb)
{
	struct oal *oal;
	struct oal_entry *oal_entry;
	int len = skb_headlen(skb);
	dma_addr_t map;
	int err;
	int completed_segs, i;
	int seg_cnt, seg = 0;
	int frag_cnt = (int)skb_shinfo(skb)->nr_frags;

	seg_cnt = tx_cb->seg_count;
	/*
	 * Map the skb buffer first.
	 */
	map = pci_map_single(qdev->pdev, skb->data, len, PCI_DMA_TODEVICE);

	err = pci_dma_mapping_error(qdev->pdev, map);
	if (err) {
		netdev_err(qdev->ndev, "PCI mapping failed with error: %d\n",
			   err);

		return NETDEV_TX_BUSY;
	}

	oal_entry = (struct oal_entry *)&mac_iocb_ptr->buf_addr0_low;
	oal_entry->dma_lo = cpu_to_le32(LS_64BITS(map));
	oal_entry->dma_hi = cpu_to_le32(MS_64BITS(map));
	oal_entry->len = cpu_to_le32(len);
	dma_unmap_addr_set(&tx_cb->map[seg], mapaddr, map);
	dma_unmap_len_set(&tx_cb->map[seg], maplen, len);
	seg++;

	if (seg_cnt == 1) {
		/* Terminate the last segment. */
		oal_entry->len |= cpu_to_le32(OAL_LAST_ENTRY);
		return NETDEV_TX_OK;
	}
	oal = tx_cb->oal;
	for (completed_segs = 0;
	     completed_segs < frag_cnt;
	     completed_segs++, seg++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[completed_segs];
		oal_entry++;
		/*
		 * Check for continuation requirements.
		 * It's strange but necessary.
		 * Continuation entry points to outbound address list.
		 */
		if ((seg == 2 && seg_cnt > 3) ||
		    (seg == 7 && seg_cnt > 8) ||
		    (seg == 12 && seg_cnt > 13) ||
		    (seg == 17 && seg_cnt > 18)) {
			map = pci_map_single(qdev->pdev, oal,
					     sizeof(struct oal),
					     PCI_DMA_TODEVICE);

			err = pci_dma_mapping_error(qdev->pdev, map);
			if (err) {
				netdev_err(qdev->ndev,
					   "PCI mapping outbound address list with error: %d\n",
					   err);
				goto map_error;
			}

			oal_entry->dma_lo = cpu_to_le32(LS_64BITS(map));
			oal_entry->dma_hi = cpu_to_le32(MS_64BITS(map));
			oal_entry->len = cpu_to_le32(sizeof(struct oal) |
						     OAL_CONT_ENTRY);
			dma_unmap_addr_set(&tx_cb->map[seg], mapaddr, map);
			dma_unmap_len_set(&tx_cb->map[seg], maplen,
					  sizeof(struct oal));
			oal_entry = (struct oal_entry *)oal;
			oal++;
			seg++;
		}

		map = skb_frag_dma_map(&qdev->pdev->dev, frag, 0, skb_frag_size(frag),
				       DMA_TO_DEVICE);

		err = dma_mapping_error(&qdev->pdev->dev, map);
		if (err) {
			netdev_err(qdev->ndev,
				   "PCI mapping frags failed with error: %d\n",
				   err);
			goto map_error;
		}

		oal_entry->dma_lo = cpu_to_le32(LS_64BITS(map));
		oal_entry->dma_hi = cpu_to_le32(MS_64BITS(map));
		oal_entry->len = cpu_to_le32(skb_frag_size(frag));
		dma_unmap_addr_set(&tx_cb->map[seg], mapaddr, map);
		dma_unmap_len_set(&tx_cb->map[seg], maplen, skb_frag_size(frag));
		}
	/* Terminate the last segment. */
	oal_entry->len |= cpu_to_le32(OAL_LAST_ENTRY);
	return NETDEV_TX_OK;

map_error:
	/* A PCI mapping failed and now we will need to back out
	 * We need to traverse through the oal's and associated pages which
	 * have been mapped and now we must unmap them to clean up properly
	 */

	seg = 1;
	oal_entry = (struct oal_entry *)&mac_iocb_ptr->buf_addr0_low;
	oal = tx_cb->oal;
	for (i = 0; i < completed_segs; i++, seg++) {
		oal_entry++;

		/*
		 * Check for continuation requirements.
		 * It's strange but necessary.
		 */

		if ((seg == 2 && seg_cnt > 3) ||
		    (seg == 7 && seg_cnt > 8) ||
		    (seg == 12 && seg_cnt > 13) ||
		    (seg == 17 && seg_cnt > 18)) {
			pci_unmap_single(qdev->pdev,
				dma_unmap_addr(&tx_cb->map[seg], mapaddr),
				dma_unmap_len(&tx_cb->map[seg], maplen),
				 PCI_DMA_TODEVICE);
			oal++;
			seg++;
		}

		pci_unmap_page(qdev->pdev,
			       dma_unmap_addr(&tx_cb->map[seg], mapaddr),
			       dma_unmap_len(&tx_cb->map[seg], maplen),
			       PCI_DMA_TODEVICE);
	}

	pci_unmap_single(qdev->pdev,
			 dma_unmap_addr(&tx_cb->map[0], mapaddr),
			 dma_unmap_addr(&tx_cb->map[0], maplen),
			 PCI_DMA_TODEVICE);

	return NETDEV_TX_BUSY;
