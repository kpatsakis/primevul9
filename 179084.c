static void peak_usb_unlink_all_urbs(struct peak_usb_device *dev)
{
	int i;

	/* free all Rx (submitted) urbs */
	usb_kill_anchored_urbs(&dev->rx_submitted);

	/* free unsubmitted Tx urbs first */
	for (i = 0; i < PCAN_USB_MAX_TX_URBS; i++) {
		struct urb *urb = dev->tx_contexts[i].urb;

		if (!urb ||
		    dev->tx_contexts[i].echo_index != PCAN_USB_MAX_TX_URBS) {
			/*
			 * this urb is already released or always submitted,
			 * let usb core free by itself
			 */
			continue;
		}

		usb_free_urb(urb);
		dev->tx_contexts[i].urb = NULL;
	}

	/* then free all submitted Tx urbs */
	usb_kill_anchored_urbs(&dev->tx_submitted);
	atomic_set(&dev->active_tx_urbs, 0);
}