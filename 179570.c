static int af9005_usb_probe(struct usb_interface *intf,
			    const struct usb_device_id *id)
{
	return dvb_usb_device_init(intf, &af9005_properties,
				  THIS_MODULE, NULL, adapter_nr);
}