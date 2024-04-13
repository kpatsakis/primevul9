static int ttusb_dec_probe(struct usb_interface *intf,
			   const struct usb_device_id *id)
{
	struct usb_device *udev;
	struct ttusb_dec *dec;
	int result;

	dprintk("%s\n", __func__);

	udev = interface_to_usbdev(intf);

	if (!(dec = kzalloc(sizeof(struct ttusb_dec), GFP_KERNEL))) {
		printk("%s: couldn't allocate memory.\n", __func__);
		return -ENOMEM;
	}

	usb_set_intfdata(intf, (void *)dec);

	switch (id->idProduct) {
	case 0x1006:
		ttusb_dec_set_model(dec, TTUSB_DEC3000S);
		break;

	case 0x1008:
		ttusb_dec_set_model(dec, TTUSB_DEC2000T);
		break;

	case 0x1009:
		ttusb_dec_set_model(dec, TTUSB_DEC2540T);
		break;
	}

	dec->udev = udev;

	result = ttusb_dec_init_usb(dec);
	if (result)
		goto err_usb;
	result = ttusb_dec_init_stb(dec);
	if (result)
		goto err_stb;
	result = ttusb_dec_init_dvb(dec);
	if (result)
		goto err_stb;

	dec->adapter.priv = dec;
	switch (id->idProduct) {
	case 0x1006:
		dec->fe = ttusbdecfe_dvbs_attach(&fe_config);
		break;

	case 0x1008:
	case 0x1009:
		dec->fe = ttusbdecfe_dvbt_attach(&fe_config);
		break;
	}

	if (dec->fe == NULL) {
		printk("dvb-ttusb-dec: A frontend driver was not found for device [%04x:%04x]\n",
		       le16_to_cpu(dec->udev->descriptor.idVendor),
		       le16_to_cpu(dec->udev->descriptor.idProduct));
	} else {
		if (dvb_register_frontend(&dec->adapter, dec->fe)) {
			printk("budget-ci: Frontend registration failed!\n");
			if (dec->fe->ops.release)
				dec->fe->ops.release(dec->fe);
			dec->fe = NULL;
		}
	}

	ttusb_dec_init_v_pes(dec);
	ttusb_dec_init_filters(dec);
	ttusb_dec_init_tasklet(dec);

	dec->active = 1;

	ttusb_dec_set_interface(dec, TTUSB_DEC_INTERFACE_IN);

	if (enable_rc)
		ttusb_init_rc(dec);

	return 0;
err_stb:
	ttusb_dec_exit_usb(dec);
err_usb:
	kfree(dec);
	return result;
}