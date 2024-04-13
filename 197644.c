static int pcan_usb_fd_decode_canmsg(struct pcan_usb_fd_if *usb_if,
				     struct pucan_msg *rx_msg)
{
	struct pucan_rx_msg *rm = (struct pucan_rx_msg *)rx_msg;
	struct peak_usb_device *dev = usb_if->dev[pucan_msg_get_channel(rm)];
	struct net_device *netdev = dev->netdev;
	struct canfd_frame *cfd;
	struct sk_buff *skb;
	const u16 rx_msg_flags = le16_to_cpu(rm->flags);

	if (rx_msg_flags & PUCAN_MSG_EXT_DATA_LEN) {
		/* CANFD frame case */
		skb = alloc_canfd_skb(netdev, &cfd);
		if (!skb)
			return -ENOMEM;

		if (rx_msg_flags & PUCAN_MSG_BITRATE_SWITCH)
			cfd->flags |= CANFD_BRS;

		if (rx_msg_flags & PUCAN_MSG_ERROR_STATE_IND)
			cfd->flags |= CANFD_ESI;

		cfd->len = can_dlc2len(get_canfd_dlc(pucan_msg_get_dlc(rm)));
	} else {
		/* CAN 2.0 frame case */
		skb = alloc_can_skb(netdev, (struct can_frame **)&cfd);
		if (!skb)
			return -ENOMEM;

		cfd->len = get_can_dlc(pucan_msg_get_dlc(rm));
	}

	cfd->can_id = le32_to_cpu(rm->can_id);

	if (rx_msg_flags & PUCAN_MSG_EXT_ID)
		cfd->can_id |= CAN_EFF_FLAG;

	if (rx_msg_flags & PUCAN_MSG_RTR)
		cfd->can_id |= CAN_RTR_FLAG;
	else
		memcpy(cfd->data, rm->d, cfd->len);

	peak_usb_netif_rx(skb, &usb_if->time_ref, le32_to_cpu(rm->ts_low));

	netdev->stats.rx_packets++;
	netdev->stats.rx_bytes += cfd->len;

	return 0;
}