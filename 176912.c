void pcan_usb_pro_restart_complete(struct urb *urb)
{
	/* can delete usb resources */
	peak_usb_async_complete(urb);

	/* notify candev and netdev */
	peak_usb_restart_complete(urb->context);
}