static void ttusb_dec_exit_usb(struct ttusb_dec *dec)
{
	int i;

	dprintk("%s\n", __func__);

	if (enable_rc) {
		/* we have to check whether the irq URB is already submitted.
		 * As the irq is submitted after the interface is changed,
		 * this is the best method i figured out.
		 * Any others?*/
		if (dec->interface == TTUSB_DEC_INTERFACE_IN)
			usb_kill_urb(dec->irq_urb);

		usb_free_urb(dec->irq_urb);

		usb_free_coherent(dec->udev, IRQ_PACKET_SIZE,
				  dec->irq_buffer, dec->irq_dma_handle);
	}

	dec->iso_stream_count = 0;

	for (i = 0; i < ISO_BUF_COUNT; i++)
		usb_kill_urb(dec->iso_urb[i]);

	ttusb_dec_free_iso_urbs(dec);
}