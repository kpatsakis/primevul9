 */
static netdev_tx_t ql3xxx_send(struct sk_buff *skb,
			       struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	struct ql_tx_buf_cb *tx_cb;
	u32 tot_len = skb->len;
	struct ob_mac_iocb_req *mac_iocb_ptr;

	if (unlikely(atomic_read(&qdev->tx_count) < 2))
		return NETDEV_TX_BUSY;

	tx_cb = &qdev->tx_buf[qdev->req_producer_index];
	tx_cb->seg_count = ql_get_seg_count(qdev,
					     skb_shinfo(skb)->nr_frags);
	if (tx_cb->seg_count == -1) {
		netdev_err(ndev, "%s: invalid segment count!\n", __func__);
		return NETDEV_TX_OK;
	}

	mac_iocb_ptr = tx_cb->queue_entry;
	memset((void *)mac_iocb_ptr, 0, sizeof(struct ob_mac_iocb_req));
	mac_iocb_ptr->opcode = qdev->mac_ob_opcode;
	mac_iocb_ptr->flags = OB_MAC_IOCB_REQ_X;
	mac_iocb_ptr->flags |= qdev->mb_bit_mask;
	mac_iocb_ptr->transaction_id = qdev->req_producer_index;
	mac_iocb_ptr->data_len = cpu_to_le16((u16) tot_len);
	tx_cb->skb = skb;
	if (qdev->device_id == QL3032_DEVICE_ID &&
	    skb->ip_summed == CHECKSUM_PARTIAL)
		ql_hw_csum_setup(skb, mac_iocb_ptr);

	if (ql_send_map(qdev, mac_iocb_ptr, tx_cb, skb) != NETDEV_TX_OK) {
		netdev_err(ndev, "%s: Could not map the segments!\n", __func__);
		return NETDEV_TX_BUSY;
	}

	wmb();
	qdev->req_producer_index++;
	if (qdev->req_producer_index == NUM_REQ_Q_ENTRIES)
		qdev->req_producer_index = 0;
	wmb();
	ql_write_common_reg_l(qdev,
			    &port_regs->CommonRegs.reqQProducerIndex,
			    qdev->req_producer_index);

	netif_printk(qdev, tx_queued, KERN_DEBUG, ndev,
		     "tx queued, slot %d, len %d\n",
		     qdev->req_producer_index, skb->len);

	atomic_dec(&qdev->tx_count);
	return NETDEV_TX_OK;