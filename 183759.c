static void yurex_interrupt(struct urb *urb)
{
	struct usb_yurex *dev = urb->context;
	unsigned char *buf = dev->int_buffer;
	int status = urb->status;
	unsigned long flags;
	int retval, i;

	switch (status) {
	case 0: /*success*/
		break;
	case -EOVERFLOW:
		dev_err(&dev->interface->dev,
			"%s - overflow with length %d, actual length is %d\n",
			__func__, YUREX_BUF_SIZE, dev->urb->actual_length);
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
	case -EILSEQ:
		/* The device is terminated, clean up */
		return;
	default:
		dev_err(&dev->interface->dev,
			"%s - unknown status received: %d\n", __func__, status);
		goto exit;
	}

	/* handle received message */
	switch (buf[0]) {
	case CMD_COUNT:
	case CMD_READ:
		if (buf[6] == CMD_EOF) {
			spin_lock_irqsave(&dev->lock, flags);
			dev->bbu = 0;
			for (i = 1; i < 6; i++) {
				dev->bbu += buf[i];
				if (i != 5)
					dev->bbu <<= 8;
			}
			dev_dbg(&dev->interface->dev, "%s count: %lld\n",
				__func__, dev->bbu);
			spin_unlock_irqrestore(&dev->lock, flags);

			kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
		}
		else
			dev_dbg(&dev->interface->dev,
				"data format error - no EOF\n");
		break;
	case CMD_ACK:
		dev_dbg(&dev->interface->dev, "%s ack: %c\n",
			__func__, buf[1]);
		wake_up_interruptible(&dev->waitq);
		break;
	}

exit:
	retval = usb_submit_urb(dev->urb, GFP_ATOMIC);
	if (retval) {
		dev_err(&dev->interface->dev, "%s - usb_submit_urb failed: %d\n",
			__func__, retval);
	}
}