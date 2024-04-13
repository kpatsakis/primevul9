static int ttusb_dec_alloc_iso_urbs(struct ttusb_dec *dec)
{
	int i;

	dprintk("%s\n", __func__);

	dec->iso_buffer = kcalloc(FRAMES_PER_ISO_BUF * ISO_BUF_COUNT,
			ISO_FRAME_SIZE, GFP_KERNEL);
	if (!dec->iso_buffer)
		return -ENOMEM;

	for (i = 0; i < ISO_BUF_COUNT; i++) {
		struct urb *urb;

		if (!(urb = usb_alloc_urb(FRAMES_PER_ISO_BUF, GFP_ATOMIC))) {
			ttusb_dec_free_iso_urbs(dec);
			return -ENOMEM;
		}

		dec->iso_urb[i] = urb;
	}

	ttusb_dec_setup_urbs(dec);

	return 0;
}