static void ttusb_dec_free_iso_urbs(struct ttusb_dec *dec)
{
	int i;

	dprintk("%s\n", __func__);

	for (i = 0; i < ISO_BUF_COUNT; i++)
		usb_free_urb(dec->iso_urb[i]);
	kfree(dec->iso_buffer);
}