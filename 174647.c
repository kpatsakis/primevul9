static void ttusb_dec_stop_iso_xfer(struct ttusb_dec *dec)
{
	int i;

	dprintk("%s\n", __func__);

	if (mutex_lock_interruptible(&dec->iso_mutex))
		return;

	dec->iso_stream_count--;

	if (!dec->iso_stream_count) {
		for (i = 0; i < ISO_BUF_COUNT; i++)
			usb_kill_urb(dec->iso_urb[i]);
	}

	mutex_unlock(&dec->iso_mutex);
}