static void ttusb_dec_disconnect(struct usb_interface *intf)
{
	struct ttusb_dec *dec = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);

	dprintk("%s\n", __func__);

	if (dec->active) {
		ttusb_dec_exit_tasklet(dec);
		ttusb_dec_exit_filters(dec);
		if(enable_rc)
			ttusb_dec_exit_rc(dec);
		ttusb_dec_exit_usb(dec);
		ttusb_dec_exit_dvb(dec);
	}

	kfree(dec);
}