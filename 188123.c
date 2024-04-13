static void usb_parse_ssp_isoc_endpoint_companion(struct device *ddev,
		int cfgno, int inum, int asnum, struct usb_host_endpoint *ep,
		unsigned char *buffer, int size)
{
	struct usb_ssp_isoc_ep_comp_descriptor *desc;

	/*
	 * The SuperSpeedPlus Isoc endpoint companion descriptor immediately
	 * follows the SuperSpeed Endpoint Companion descriptor
	 */
	desc = (struct usb_ssp_isoc_ep_comp_descriptor *) buffer;
	if (desc->bDescriptorType != USB_DT_SSP_ISOC_ENDPOINT_COMP ||
	    size < USB_DT_SSP_ISOC_EP_COMP_SIZE) {
		dev_warn(ddev, "Invalid SuperSpeedPlus isoc endpoint companion"
			 "for config %d interface %d altsetting %d ep %d.\n",
			 cfgno, inum, asnum, ep->desc.bEndpointAddress);
		return;
	}
	memcpy(&ep->ssp_isoc_ep_comp, desc, USB_DT_SSP_ISOC_EP_COMP_SIZE);
}