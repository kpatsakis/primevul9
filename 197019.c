static void ems_usb_disconnect(struct usb_interface *intf)
{
	struct ems_usb *dev = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);

	if (dev) {
		unregister_netdev(dev->netdev);

		unlink_all_urbs(dev);

		usb_free_urb(dev->intr_urb);

		kfree(dev->intr_in_buffer);
		kfree(dev->tx_msg_buffer);

		free_candev(dev->netdev);
	}
}