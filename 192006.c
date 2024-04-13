static int rtl8xxxu_parse_usb(struct rtl8xxxu_priv *priv,
			      struct usb_interface *interface)
{
	struct usb_interface_descriptor *interface_desc;
	struct usb_host_interface *host_interface;
	struct usb_endpoint_descriptor *endpoint;
	struct device *dev = &priv->udev->dev;
	int i, j = 0, endpoints;
	u8 dir, xtype, num;
	int ret = 0;

	host_interface = &interface->altsetting[0];
	interface_desc = &host_interface->desc;
	endpoints = interface_desc->bNumEndpoints;

	for (i = 0; i < endpoints; i++) {
		endpoint = &host_interface->endpoint[i].desc;

		dir = endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK;
		num = usb_endpoint_num(endpoint);
		xtype = usb_endpoint_type(endpoint);
		if (rtl8xxxu_debug & RTL8XXXU_DEBUG_USB)
			dev_dbg(dev,
				"%s: endpoint: dir %02x, # %02x, type %02x\n",
				__func__, dir, num, xtype);
		if (usb_endpoint_dir_in(endpoint) &&
		    usb_endpoint_xfer_bulk(endpoint)) {
			if (rtl8xxxu_debug & RTL8XXXU_DEBUG_USB)
				dev_dbg(dev, "%s: in endpoint num %i\n",
					__func__, num);

			if (priv->pipe_in) {
				dev_warn(dev,
					 "%s: Too many IN pipes\n", __func__);
				ret = -EINVAL;
				goto exit;
			}

			priv->pipe_in =	usb_rcvbulkpipe(priv->udev, num);
		}

		if (usb_endpoint_dir_in(endpoint) &&
		    usb_endpoint_xfer_int(endpoint)) {
			if (rtl8xxxu_debug & RTL8XXXU_DEBUG_USB)
				dev_dbg(dev, "%s: interrupt endpoint num %i\n",
					__func__, num);

			if (priv->pipe_interrupt) {
				dev_warn(dev, "%s: Too many INTERRUPT pipes\n",
					 __func__);
				ret = -EINVAL;
				goto exit;
			}

			priv->pipe_interrupt = usb_rcvintpipe(priv->udev, num);
		}

		if (usb_endpoint_dir_out(endpoint) &&
		    usb_endpoint_xfer_bulk(endpoint)) {
			if (rtl8xxxu_debug & RTL8XXXU_DEBUG_USB)
				dev_dbg(dev, "%s: out endpoint num %i\n",
					__func__, num);
			if (j >= RTL8XXXU_OUT_ENDPOINTS) {
				dev_warn(dev,
					 "%s: Too many OUT pipes\n", __func__);
				ret = -EINVAL;
				goto exit;
			}
			priv->out_ep[j++] = num;
		}
	}
exit:
	priv->nr_out_eps = j;
	return ret;
}