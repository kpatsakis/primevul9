 */
static void ql_process_mac_rx_intr(struct ql3_adapter *qdev,
				   struct ib_mac_iocb_rsp *ib_mac_rsp_ptr)
{
	struct ql_rcv_buf_cb *lrg_buf_cb1 = NULL;
	struct ql_rcv_buf_cb *lrg_buf_cb2 = NULL;
	struct sk_buff *skb;
	u16 length = le16_to_cpu(ib_mac_rsp_ptr->length);

	/*
	 * Get the inbound address list (small buffer).
	 */
	ql_get_sbuf(qdev);

	if (qdev->device_id == QL3022_DEVICE_ID)
		lrg_buf_cb1 = ql_get_lbuf(qdev);

	/* start of second buffer */
	lrg_buf_cb2 = ql_get_lbuf(qdev);
	skb = lrg_buf_cb2->skb;

	qdev->ndev->stats.rx_packets++;
	qdev->ndev->stats.rx_bytes += length;

	skb_put(skb, length);
	pci_unmap_single(qdev->pdev,
			 dma_unmap_addr(lrg_buf_cb2, mapaddr),
			 dma_unmap_len(lrg_buf_cb2, maplen),
			 PCI_DMA_FROMDEVICE);
	prefetch(skb->data);
	skb_checksum_none_assert(skb);
	skb->protocol = eth_type_trans(skb, qdev->ndev);

	napi_gro_receive(&qdev->napi, skb);
	lrg_buf_cb2->skb = NULL;

	if (qdev->device_id == QL3022_DEVICE_ID)
		ql_release_to_lrg_buf_free_list(qdev, lrg_buf_cb1);
	ql_release_to_lrg_buf_free_list(qdev, lrg_buf_cb2);