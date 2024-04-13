static netdev_tx_t peak_usb_ndo_start_xmit(struct sk_buff *skb,
					   struct net_device *netdev)
{
	struct peak_usb_device *dev = netdev_priv(netdev);
	struct peak_tx_urb_context *context = NULL;
	struct net_device_stats *stats = &netdev->stats;
	struct canfd_frame *cfd = (struct canfd_frame *)skb->data;
	struct urb *urb;
	u8 *obuf;
	int i, err;
	size_t size = dev->adapter->tx_buffer_size;

	if (can_dropped_invalid_skb(netdev, skb))
		return NETDEV_TX_OK;

	for (i = 0; i < PCAN_USB_MAX_TX_URBS; i++)
		if (dev->tx_contexts[i].echo_index == PCAN_USB_MAX_TX_URBS) {
			context = dev->tx_contexts + i;
			break;
		}

	if (!context) {
		/* should not occur except during restart */
		return NETDEV_TX_BUSY;
	}

	urb = context->urb;
	obuf = urb->transfer_buffer;

	err = dev->adapter->dev_encode_msg(dev, skb, obuf, &size);
	if (err) {
		if (net_ratelimit())
			netdev_err(netdev, "packet dropped\n");
		dev_kfree_skb(skb);
		stats->tx_dropped++;
		return NETDEV_TX_OK;
	}

	context->echo_index = i;

	/* Note: this works with CANFD frames too */
	context->data_len = cfd->len;

	usb_anchor_urb(urb, &dev->tx_submitted);

	can_put_echo_skb(skb, netdev, context->echo_index);

	atomic_inc(&dev->active_tx_urbs);

	err = usb_submit_urb(urb, GFP_ATOMIC);
	if (err) {
		can_free_echo_skb(netdev, context->echo_index);

		usb_unanchor_urb(urb);

		/* this context is not used in fact */
		context->echo_index = PCAN_USB_MAX_TX_URBS;

		atomic_dec(&dev->active_tx_urbs);

		switch (err) {
		case -ENODEV:
			netif_device_detach(netdev);
			break;
		default:
			netdev_warn(netdev, "tx urb submitting failed err=%d\n",
				    err);
			/* fall through */
		case -ENOENT:
			/* cable unplugged */
			stats->tx_dropped++;
		}
	} else {
		netif_trans_update(netdev);

		/* slow down tx path */
		if (atomic_read(&dev->active_tx_urbs) >= PCAN_USB_MAX_TX_URBS)
			netif_stop_queue(netdev);
	}

	return NETDEV_TX_OK;
}