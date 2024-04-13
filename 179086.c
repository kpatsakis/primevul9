static int peak_usb_start(struct peak_usb_device *dev)
{
	struct net_device *netdev = dev->netdev;
	int err, i;

	for (i = 0; i < PCAN_USB_MAX_RX_URBS; i++) {
		struct urb *urb;
		u8 *buf;

		/* create a URB, and a buffer for it, to receive usb messages */
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb) {
			err = -ENOMEM;
			break;
		}

		buf = kmalloc(dev->adapter->rx_buffer_size, GFP_KERNEL);
		if (!buf) {
			usb_free_urb(urb);
			err = -ENOMEM;
			break;
		}

		usb_fill_bulk_urb(urb, dev->udev,
			usb_rcvbulkpipe(dev->udev, dev->ep_msg_in),
			buf, dev->adapter->rx_buffer_size,
			peak_usb_read_bulk_callback, dev);

		/* ask last usb_free_urb() to also kfree() transfer_buffer */
		urb->transfer_flags |= URB_FREE_BUFFER;
		usb_anchor_urb(urb, &dev->rx_submitted);

		err = usb_submit_urb(urb, GFP_KERNEL);
		if (err) {
			if (err == -ENODEV)
				netif_device_detach(dev->netdev);

			usb_unanchor_urb(urb);
			kfree(buf);
			usb_free_urb(urb);
			break;
		}

		/* drop reference, USB core will take care of freeing it */
		usb_free_urb(urb);
	}

	/* did we submit any URBs? Warn if we was not able to submit all urbs */
	if (i < PCAN_USB_MAX_RX_URBS) {
		if (i == 0) {
			netdev_err(netdev, "couldn't setup any rx URB\n");
			return err;
		}

		netdev_warn(netdev, "rx performance may be slow\n");
	}

	/* pre-alloc tx buffers and corresponding urbs */
	for (i = 0; i < PCAN_USB_MAX_TX_URBS; i++) {
		struct peak_tx_urb_context *context;
		struct urb *urb;
		u8 *buf;

		/* create a URB and a buffer for it, to transmit usb messages */
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb) {
			err = -ENOMEM;
			break;
		}

		buf = kmalloc(dev->adapter->tx_buffer_size, GFP_KERNEL);
		if (!buf) {
			usb_free_urb(urb);
			err = -ENOMEM;
			break;
		}

		context = dev->tx_contexts + i;
		context->dev = dev;
		context->urb = urb;

		usb_fill_bulk_urb(urb, dev->udev,
			usb_sndbulkpipe(dev->udev, dev->ep_msg_out),
			buf, dev->adapter->tx_buffer_size,
			peak_usb_write_bulk_callback, context);

		/* ask last usb_free_urb() to also kfree() transfer_buffer */
		urb->transfer_flags |= URB_FREE_BUFFER;
	}

	/* warn if we were not able to allocate enough tx contexts */
	if (i < PCAN_USB_MAX_TX_URBS) {
		if (i == 0) {
			netdev_err(netdev, "couldn't setup any tx URB\n");
			goto err_tx;
		}

		netdev_warn(netdev, "tx performance may be slow\n");
	}

	if (dev->adapter->dev_start) {
		err = dev->adapter->dev_start(dev);
		if (err)
			goto err_adapter;
	}

	dev->state |= PCAN_USB_STATE_STARTED;

	/* can set bus on now */
	if (dev->adapter->dev_set_bus) {
		err = dev->adapter->dev_set_bus(dev, 1);
		if (err)
			goto err_adapter;
	}

	dev->can.state = CAN_STATE_ERROR_ACTIVE;

	return 0;

err_adapter:
	if (err == -ENODEV)
		netif_device_detach(dev->netdev);

	netdev_warn(netdev, "couldn't submit control: %d\n", err);

	for (i = 0; i < PCAN_USB_MAX_TX_URBS; i++) {
		usb_free_urb(dev->tx_contexts[i].urb);
		dev->tx_contexts[i].urb = NULL;
	}
err_tx:
	usb_kill_anchored_urbs(&dev->rx_submitted);

	return err;
}