
static int ql_populate_free_queue(struct ql3_adapter *qdev)
{
	struct ql_rcv_buf_cb *lrg_buf_cb = qdev->lrg_buf_free_head;
	dma_addr_t map;
	int err;

	while (lrg_buf_cb) {
		if (!lrg_buf_cb->skb) {
			lrg_buf_cb->skb =
				netdev_alloc_skb(qdev->ndev,
						 qdev->lrg_buffer_len);
			if (unlikely(!lrg_buf_cb->skb)) {
				netdev_printk(KERN_DEBUG, qdev->ndev,
					      "Failed netdev_alloc_skb()\n");
				break;
			} else {
				/*
				 * We save some space to copy the ethhdr from
				 * first buffer
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
					break;
				}


				lrg_buf_cb->buf_phy_addr_low =
					cpu_to_le32(LS_64BITS(map));
				lrg_buf_cb->buf_phy_addr_high =
					cpu_to_le32(MS_64BITS(map));
				dma_unmap_addr_set(lrg_buf_cb, mapaddr, map);
				dma_unmap_len_set(lrg_buf_cb, maplen,
						  qdev->lrg_buffer_len -
						  QL_HEADER_SPACE);
				--qdev->lrg_buf_skb_check;
				if (!qdev->lrg_buf_skb_check)
					return 1;
			}
		}
		lrg_buf_cb = lrg_buf_cb->next;
	}
	return 0;