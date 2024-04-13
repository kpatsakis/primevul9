static void yurex_control_callback(struct urb *urb)
{
	struct usb_yurex *dev = urb->context;
	int status = urb->status;

	if (status) {
		dev_err(&urb->dev->dev, "%s - control failed: %d\n",
			__func__, status);
		wake_up_interruptible(&dev->waitq);
		return;
	}
	/* on success, sender woken up by CMD_ACK int in, or timeout */
}