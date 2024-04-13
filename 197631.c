static int pcan_usb_fd_decode_status(struct pcan_usb_fd_if *usb_if,
				     struct pucan_msg *rx_msg)
{
	struct pucan_status_msg *sm = (struct pucan_status_msg *)rx_msg;
	struct peak_usb_device *dev = usb_if->dev[pucan_stmsg_get_channel(sm)];
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);
	enum can_state new_state = CAN_STATE_ERROR_ACTIVE;
	enum can_state rx_state, tx_state;
	struct net_device *netdev = dev->netdev;
	struct can_frame *cf;
	struct sk_buff *skb;

	/* nothing should be sent while in BUS_OFF state */
	if (dev->can.state == CAN_STATE_BUS_OFF)
		return 0;

	if (sm->channel_p_w_b & PUCAN_BUS_BUSOFF) {
		new_state = CAN_STATE_BUS_OFF;
	} else if (sm->channel_p_w_b & PUCAN_BUS_PASSIVE) {
		new_state = CAN_STATE_ERROR_PASSIVE;
	} else if (sm->channel_p_w_b & PUCAN_BUS_WARNING) {
		new_state = CAN_STATE_ERROR_WARNING;
	} else {
		/* no error bit (so, no error skb, back to active state) */
		dev->can.state = CAN_STATE_ERROR_ACTIVE;
		pdev->bec.txerr = 0;
		pdev->bec.rxerr = 0;
		return 0;
	}

	/* state hasn't changed */
	if (new_state == dev->can.state)
		return 0;

	/* handle bus state change */
	tx_state = (pdev->bec.txerr >= pdev->bec.rxerr) ? new_state : 0;
	rx_state = (pdev->bec.txerr <= pdev->bec.rxerr) ? new_state : 0;

	/* allocate an skb to store the error frame */
	skb = alloc_can_err_skb(netdev, &cf);
	if (skb)
		can_change_state(netdev, cf, tx_state, rx_state);

	/* things must be done even in case of OOM */
	if (new_state == CAN_STATE_BUS_OFF)
		can_bus_off(netdev);

	if (!skb)
		return -ENOMEM;

	peak_usb_netif_rx(skb, &usb_if->time_ref, le32_to_cpu(sm->ts_low));

	netdev->stats.rx_packets++;
	netdev->stats.rx_bytes += cf->can_dlc;

	return 0;
}