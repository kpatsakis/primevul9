static int pcan_usb_fd_decode_overrun(struct pcan_usb_fd_if *usb_if,
				      struct pucan_msg *rx_msg)
{
	struct pcan_ufd_ovr_msg *ov = (struct pcan_ufd_ovr_msg *)rx_msg;
	struct peak_usb_device *dev = usb_if->dev[pufd_omsg_get_channel(ov)];
	struct net_device *netdev = dev->netdev;
	struct can_frame *cf;
	struct sk_buff *skb;

	/* allocate an skb to store the error frame */
	skb = alloc_can_err_skb(netdev, &cf);
	if (!skb)
		return -ENOMEM;

	cf->can_id |= CAN_ERR_CRTL;
	cf->data[1] |= CAN_ERR_CRTL_RX_OVERFLOW;

	peak_usb_netif_rx(skb, &usb_if->time_ref, le32_to_cpu(ov->ts_low));

	netdev->stats.rx_over_errors++;
	netdev->stats.rx_errors++;

	return 0;
}