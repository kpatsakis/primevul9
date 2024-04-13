
static void ql_process_macip_rx_intr(struct ql3_adapter *qdev,
				     struct ib_ip_iocb_rsp *ib_ip_rsp_ptr)
{
	struct ql_rcv_buf_cb *lrg_buf_cb1 = NULL;
	struct ql_rcv_buf_cb *lrg_buf_cb2 = NULL;
	struct sk_buff *skb1 = NULL, *skb2;
	struct net_device *ndev = qdev->ndev;
	u16 length = le16_to_cpu(ib_ip_rsp_ptr->length);
	u16 size = 0;

	/*
	 * Get the inbound address list (small buffer).
	 */

	ql_get_sbuf(qdev);

	if (qdev->device_id == QL3022_DEVICE_ID) {
		/* start of first buffer on 3022 */
		lrg_buf_cb1 = ql_get_lbuf(qdev);
		skb1 = lrg_buf_cb1->skb;
		size = ETH_HLEN;
		if (*((u16 *) skb1->data) != 0xFFFF)
			size += VLAN_ETH_HLEN - ETH_HLEN;
	}

	/* start of second buffer */
	lrg_buf_cb2 = ql_get_lbuf(qdev);
	skb2 = lrg_buf_cb2->skb;

	skb_put(skb2, length);	/* Just the second buffer length here. */
	pci_unmap_single(qdev->pdev,
			 dma_unmap_addr(lrg_buf_cb2, mapaddr),
			 dma_unmap_len(lrg_buf_cb2, maplen),
			 PCI_DMA_FROMDEVICE);
	prefetch(skb2->data);

	skb_checksum_none_assert(skb2);
	if (qdev->device_id == QL3022_DEVICE_ID) {
		/*
		 * Copy the ethhdr from first buffer to second. This
		 * is necessary for 3022 IP completions.
		 */
		skb_copy_from_linear_data_offset(skb1, VLAN_ID_LEN,
						 skb_push(skb2, size), size);
	} else {
		u16 checksum = le16_to_cpu(ib_ip_rsp_ptr->checksum);
		if (checksum &
			(IB_IP_IOCB_RSP_3032_ICE |
			 IB_IP_IOCB_RSP_3032_CE)) {
			netdev_err(ndev,
				   "%s: Bad checksum for this %s packet, checksum = %x\n",
				   __func__,
				   ((checksum & IB_IP_IOCB_RSP_3032_TCP) ?
				    "TCP" : "UDP"), checksum);
		} else if ((checksum & IB_IP_IOCB_RSP_3032_TCP) ||
				(checksum & IB_IP_IOCB_RSP_3032_UDP &&
				!(checksum & IB_IP_IOCB_RSP_3032_NUC))) {
			skb2->ip_summed = CHECKSUM_UNNECESSARY;
		}
	}
	skb2->protocol = eth_type_trans(skb2, qdev->ndev);

	napi_gro_receive(&qdev->napi, skb2);
	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += length;
	lrg_buf_cb2->skb = NULL;

	if (qdev->device_id == QL3022_DEVICE_ID)
		ql_release_to_lrg_buf_free_list(qdev, lrg_buf_cb1);
	ql_release_to_lrg_buf_free_list(qdev, lrg_buf_cb2);