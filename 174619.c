static void ttusb_dec_handle_irq( struct urb *urb)
{
	struct ttusb_dec *dec = urb->context;
	char *buffer = dec->irq_buffer;
	int retval;

	switch(urb->status) {
		case 0: /*success*/
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN:
		case -ETIME:
			/* this urb is dead, cleanup */
			dprintk("%s:urb shutting down with status: %d\n",
					__func__, urb->status);
			return;
		default:
			dprintk("%s:nonzero status received: %d\n",
					__func__,urb->status);
			goto exit;
	}

	if ((buffer[0] == 0x1) && (buffer[2] == 0x15))  {
		/*
		 * IR - Event
		 *
		 * this is an fact a bit too simple implementation;
		 * the box also reports a keyrepeat signal
		 * (with buffer[3] == 0x40) in an interval of ~100ms.
		 * But to handle this correctly we had to imlemenent some
		 * kind of timer which signals a 'key up' event if no
		 * keyrepeat signal is received for lets say 200ms.
		 * this should/could be added later ...
		 * for now lets report each signal as a key down and up
		 */
		if (buffer[4] - 1 < ARRAY_SIZE(rc_keys)) {
			dprintk("%s:rc signal:%d\n", __func__, buffer[4]);
			input_report_key(dec->rc_input_dev, rc_keys[buffer[4] - 1], 1);
			input_sync(dec->rc_input_dev);
			input_report_key(dec->rc_input_dev, rc_keys[buffer[4] - 1], 0);
			input_sync(dec->rc_input_dev);
		}
	}

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		printk("%s - usb_commit_urb failed with result: %d\n",
			__func__, retval);
}