static void unlink_all_urbs(struct ems_usb *dev)
{
	int i;

	usb_unlink_urb(dev->intr_urb);

	usb_kill_anchored_urbs(&dev->rx_submitted);

	for (i = 0; i < MAX_RX_URBS; ++i)
		usb_free_coherent(dev->udev, RX_BUFFER_SIZE,
				  dev->rxbuf[i], dev->rxbuf_dma[i]);

	usb_kill_anchored_urbs(&dev->tx_submitted);
	atomic_set(&dev->active_tx_urbs, 0);

	for (i = 0; i < MAX_TX_URBS; i++)
		dev->tx_contexts[i].echo_index = MAX_TX_URBS;
}