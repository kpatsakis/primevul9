static void pn533_usb_abort_cmd(struct pn533 *dev, gfp_t flags)
{
	struct pn533_usb_phy *phy = dev->phy;

	/* ACR122U does not support any command which aborts last
	 * issued command i.e. as ACK for standard PN533. Additionally,
	 * it behaves stange, sending broken or incorrect responses,
	 * when we cancel urb before the chip will send response.
	 */
	if (dev->device_type == PN533_DEVICE_ACR122U)
		return;

	/* An ack will cancel the last issued command */
	pn533_usb_send_ack(dev, flags);

	/* cancel the urb request */
	usb_kill_urb(phy->in_urb);
}