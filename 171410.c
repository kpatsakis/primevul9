
static void ql_process_mac_tx_intr(struct ql3_adapter *qdev,
				   struct ob_mac_iocb_rsp *mac_rsp)
{
	struct ql_tx_buf_cb *tx_cb;
	int i;

	if (mac_rsp->flags & OB_MAC_IOCB_RSP_S) {
		netdev_warn(qdev->ndev,
			    "Frame too short but it was padded and sent\n");
	}

	tx_cb = &qdev->tx_buf[mac_rsp->transaction_id];

	/*  Check the transmit response flags for any errors */
	if (mac_rsp->flags & OB_MAC_IOCB_RSP_S) {
		netdev_err(qdev->ndev,
			   "Frame too short to be legal, frame not sent\n");

		qdev->ndev->stats.tx_errors++;
		goto frame_not_sent;
	}

	if (tx_cb->seg_count == 0) {
		netdev_err(qdev->ndev, "tx_cb->seg_count == 0: %d\n",
			   mac_rsp->transaction_id);

		qdev->ndev->stats.tx_errors++;
		goto invalid_seg_count;
	}

	pci_unmap_single(qdev->pdev,
			 dma_unmap_addr(&tx_cb->map[0], mapaddr),
			 dma_unmap_len(&tx_cb->map[0], maplen),
			 PCI_DMA_TODEVICE);
	tx_cb->seg_count--;
	if (tx_cb->seg_count) {
		for (i = 1; i < tx_cb->seg_count; i++) {
			pci_unmap_page(qdev->pdev,
				       dma_unmap_addr(&tx_cb->map[i],
						      mapaddr),
				       dma_unmap_len(&tx_cb->map[i], maplen),
				       PCI_DMA_TODEVICE);
		}
	}
	qdev->ndev->stats.tx_packets++;
	qdev->ndev->stats.tx_bytes += tx_cb->skb->len;

frame_not_sent:
	dev_kfree_skb_irq(tx_cb->skb);
	tx_cb->skb = NULL;

invalid_seg_count:
	atomic_inc(&qdev->tx_count);