static int ems_usb_start(struct ems_usb *dev)
{
	struct net_device *netdev = dev->netdev;
	int err, i;

	dev->intr_in_buffer[0] = 0;
	dev->free_slots = 50; /* initial size */

	for (i = 0; i < MAX_RX_URBS; i++) {
		struct urb *urb = NULL;
		u8 *buf = NULL;
		dma_addr_t buf_dma;

		/* create a URB, and a buffer for it */
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb) {
			err = -ENOMEM;
			break;
		}

		buf = usb_alloc_coherent(dev->udev, RX_BUFFER_SIZE, GFP_KERNEL,
					 &buf_dma);
		if (!buf) {
			netdev_err(netdev, "No memory left for USB buffer\n");
			usb_free_urb(urb);
			err = -ENOMEM;
			break;
		}

		urb->transfer_dma = buf_dma;

		usb_fill_bulk_urb(urb, dev->udev, usb_rcvbulkpipe(dev->udev, 2),
				  buf, RX_BUFFER_SIZE,
				  ems_usb_read_bulk_callback, dev);
		urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		usb_anchor_urb(urb, &dev->rx_submitted);

		err = usb_submit_urb(urb, GFP_KERNEL);
		if (err) {
			usb_unanchor_urb(urb);
			usb_free_coherent(dev->udev, RX_BUFFER_SIZE, buf,
					  urb->transfer_dma);
			usb_free_urb(urb);
			break;
		}

		dev->rxbuf[i] = buf;
		dev->rxbuf_dma[i] = buf_dma;

		/* Drop reference, USB core will take care of freeing it */
		usb_free_urb(urb);
	}

	/* Did we submit any URBs */
	if (i == 0) {
		netdev_warn(netdev, "couldn't setup read URBs\n");
		return err;
	}

	/* Warn if we've couldn't transmit all the URBs */
	if (i < MAX_RX_URBS)
		netdev_warn(netdev, "rx performance may be slow\n");

	/* Setup and start interrupt URB */
	usb_fill_int_urb(dev->intr_urb, dev->udev,
			 usb_rcvintpipe(dev->udev, 1),
			 dev->intr_in_buffer,
			 INTR_IN_BUFFER_SIZE,
			 ems_usb_read_interrupt_callback, dev, 1);

	err = usb_submit_urb(dev->intr_urb, GFP_KERNEL);
	if (err) {
		netdev_warn(netdev, "intr URB submit failed: %d\n", err);

		return err;
	}

	/* CPC-USB will transfer received message to host */
	err = ems_usb_control_cmd(dev, CONTR_CAN_MESSAGE | CONTR_CONT_ON);
	if (err)
		goto failed;

	/* CPC-USB will transfer CAN state changes to host */
	err = ems_usb_control_cmd(dev, CONTR_CAN_STATE | CONTR_CONT_ON);
	if (err)
		goto failed;

	/* CPC-USB will transfer bus errors to host */
	err = ems_usb_control_cmd(dev, CONTR_BUS_ERROR | CONTR_CONT_ON);
	if (err)
		goto failed;

	err = ems_usb_write_mode(dev, SJA1000_MOD_NORMAL);
	if (err)
		goto failed;

	dev->can.state = CAN_STATE_ERROR_ACTIVE;

	return 0;

failed:
	netdev_warn(netdev, "couldn't submit control: %d\n", err);

	return err;
}