static void pn533_usb_disconnect(struct usb_interface *interface)
{
	struct pn533_usb_phy *phy = usb_get_intfdata(interface);

	if (!phy)
		return;

	pn533_unregister_device(phy->priv);

	usb_set_intfdata(interface, NULL);

	usb_kill_urb(phy->in_urb);
	usb_kill_urb(phy->out_urb);
	usb_kill_urb(phy->ack_urb);

	kfree(phy->in_urb->transfer_buffer);
	usb_free_urb(phy->in_urb);
	usb_free_urb(phy->out_urb);
	usb_free_urb(phy->ack_urb);
	kfree(phy->ack_buffer);

	nfc_info(&interface->dev, "NXP PN533 NFC device disconnected\n");
}