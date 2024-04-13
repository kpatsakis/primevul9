static int ttusb_dec_set_interface(struct ttusb_dec *dec,
				   enum ttusb_dec_interface interface)
{
	int result = 0;
	u8 b[] = { 0x05 };

	if (interface != dec->interface) {
		switch (interface) {
		case TTUSB_DEC_INTERFACE_INITIAL:
			result = usb_set_interface(dec->udev, 0, 0);
			break;
		case TTUSB_DEC_INTERFACE_IN:
			result = ttusb_dec_send_command(dec, 0x80, sizeof(b),
							b, NULL, NULL);
			if (result)
				return result;
			result = usb_set_interface(dec->udev, 0, 8);
			break;
		case TTUSB_DEC_INTERFACE_OUT:
			result = usb_set_interface(dec->udev, 0, 1);
			break;
		}

		if (result)
			return result;

		dec->interface = interface;
	}

	return 0;
}