static void ql_release_to_lrg_buf_free_list(struct ql3_adapter *qdev,
					    struct ql_rcv_buf_cb *lrg_buf_cb)
{
	dma_addr_t map;
	int err;
	lrg_buf_cb->next = NULL;

	if (qdev->lrg_buf_free_tail == NULL) {	/* The list is empty  */
		qdev->lrg_buf_free_head = qdev->lrg_buf_free_tail = lrg_buf_cb;
	} else {
		qdev->lrg_buf_free_tail->next = lrg_buf_cb;
		qdev->lrg_buf_free_tail = lrg_buf_cb;
	}

	if (!lrg_buf_cb->skb) {
		lrg_buf_cb->skb = netdev_alloc_skb(qdev->ndev,
						   qdev->lrg_buffer_len);
		if (unlikely(!lrg_buf_cb->skb)) {
			qdev->lrg_buf_skb_check++;
		} else {
			/*
			 * We save some space to copy the ethhdr from first
			 * buffer
			 */
			skb_reserve(lrg_buf_cb->skb, QL_HEADER_SPACE);
			map = pci_map_single(qdev->pdev,
					     lrg_buf_cb->skb->data,
					     qdev->lrg_buffer_len -
					     QL_HEADER_SPACE,
					     PCI_DMA_FROMDEVICE);
			err = pci_dma_mapping_error(qdev->pdev, map);
			if (err) {
				netdev_err(qdev->ndev,
					   "PCI mapping failed with error: %d\n",
					   err);
				dev_kfree_skb(lrg_buf_cb->skb);
				lrg_buf_cb->skb = NULL;

				qdev->lrg_buf_skb_check++;
				return;
			}

			lrg_buf_cb->buf_phy_addr_low =
			    cpu_to_le32(LS_64BITS(map));
			lrg_buf_cb->buf_phy_addr_high =
			    cpu_to_le32(MS_64BITS(map));
			dma_unmap_addr_set(lrg_buf_cb, mapaddr, map);
			dma_unmap_len_set(lrg_buf_cb, maplen,
					  qdev->lrg_buffer_len -
					  QL_HEADER_SPACE);
		}
	}

	qdev->lrg_buf_free_count++;
}