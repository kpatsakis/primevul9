int pcan_usb_pro_probe(struct usb_interface *intf)
{
	struct usb_host_interface *if_desc;
	int i;

	if_desc = intf->altsetting;

	/* check interface endpoint addresses */
	for (i = 0; i < if_desc->desc.bNumEndpoints; i++) {
		struct usb_endpoint_descriptor *ep = &if_desc->endpoint[i].desc;

		/*
		 * below is the list of valid ep addreses. Any other ep address
		 * is considered as not-CAN interface address => no dev created
		 */
		switch (ep->bEndpointAddress) {
		case PCAN_USBPRO_EP_CMDOUT:
		case PCAN_USBPRO_EP_CMDIN:
		case PCAN_USBPRO_EP_MSGOUT_0:
		case PCAN_USBPRO_EP_MSGOUT_1:
		case PCAN_USBPRO_EP_MSGIN:
		case PCAN_USBPRO_EP_UNUSED:
			break;
		default:
			return -ENODEV;
		}
	}

	return 0;
}