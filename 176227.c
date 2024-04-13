static void ax88179_status(struct usbnet *dev, struct urb *urb)
{
	struct ax88179_int_data *event;
	u32 link;

	if (urb->actual_length < 8)
		return;

	event = urb->transfer_buffer;
	le32_to_cpus((void *)&event->intdata1);

	link = (((__force u32)event->intdata1) & AX_INT_PPLS_LINK) >> 16;

	if (netif_carrier_ok(dev->net) != link) {
		usbnet_link_change(dev, link, 1);
		netdev_info(dev->net, "ax88179 - Link status is: %d\n", link);
	}
}