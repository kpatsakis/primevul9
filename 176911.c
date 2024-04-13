static int pcan_usb_pro_handle_canmsg(struct pcan_usb_pro_interface *usb_if,
				      struct pcan_usb_pro_rxmsg *rx)
{
	const unsigned int ctrl_idx = (rx->len >> 4) & 0x0f;
	struct peak_usb_device *dev = usb_if->dev[ctrl_idx];
	struct net_device *netdev = dev->netdev;
	struct can_frame *can_frame;
	struct sk_buff *skb;
	struct skb_shared_hwtstamps *hwts;

	skb = alloc_can_skb(netdev, &can_frame);
	if (!skb)
		return -ENOMEM;

	can_frame->can_id = le32_to_cpu(rx->id);
	can_frame->can_dlc = rx->len & 0x0f;

	if (rx->flags & PCAN_USBPRO_EXT)
		can_frame->can_id |= CAN_EFF_FLAG;

	if (rx->flags & PCAN_USBPRO_RTR)
		can_frame->can_id |= CAN_RTR_FLAG;
	else
		memcpy(can_frame->data, rx->data, can_frame->can_dlc);

	hwts = skb_hwtstamps(skb);
	peak_usb_get_ts_time(&usb_if->time_ref, le32_to_cpu(rx->ts32),
			     &hwts->hwtstamp);

	netdev->stats.rx_packets++;
	netdev->stats.rx_bytes += can_frame->can_dlc;
	netif_rx(skb);

	return 0;
}