static int raremono_cmd_main(struct raremono_device *radio, unsigned band, unsigned freq)
{
	unsigned band_offset;
	int ret;

	switch (band) {
	case BAND_FM:
		band_offset = 1;
		freq /= 10;
		break;
	case BAND_AM:
		band_offset = 0;
		break;
	default:
		band_offset = 2;
		break;
	}
	radio->buffer[0] = 0x04 + band_offset;
	radio->buffer[1] = freq >> 8;
	radio->buffer[2] = freq & 0xff;

	ret = usb_control_msg(radio->usbdev, usb_sndctrlpipe(radio->usbdev, 0),
			HID_REQ_SET_REPORT,
			USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
			0x0300 + radio->buffer[0], 2,
			radio->buffer, 3, USB_TIMEOUT);

	if (ret < 0) {
		dev_warn(radio->v4l2_dev.dev, "%s failed (%d)\n", __func__, ret);
		return ret;
	}
	radio->curfreq = (band == BAND_FM) ? freq * 10 : freq;
	return 0;
}