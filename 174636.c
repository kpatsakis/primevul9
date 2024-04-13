static int ttusb_dec_start_iso_xfer(struct ttusb_dec *dec)
{
	int i, result;

	dprintk("%s\n", __func__);

	if (mutex_lock_interruptible(&dec->iso_mutex))
		return -EAGAIN;

	if (!dec->iso_stream_count) {
		ttusb_dec_setup_urbs(dec);

		dec->packet_state = 0;
		dec->v_pes_postbytes = 0;
		dec->next_packet_id = 0;

		for (i = 0; i < ISO_BUF_COUNT; i++) {
			if ((result = usb_submit_urb(dec->iso_urb[i],
						     GFP_ATOMIC))) {
				printk("%s: failed urb submission %d: error %d\n",
				       __func__, i, result);

				while (i) {
					usb_kill_urb(dec->iso_urb[i - 1]);
					i--;
				}

				mutex_unlock(&dec->iso_mutex);
				return result;
			}
		}
	}

	dec->iso_stream_count++;

	mutex_unlock(&dec->iso_mutex);

	return 0;
}