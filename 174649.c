static void ttusb_dec_process_urb(struct urb *urb)
{
	struct ttusb_dec *dec = urb->context;

	if (!urb->status) {
		int i;

		for (i = 0; i < FRAMES_PER_ISO_BUF; i++) {
			struct usb_iso_packet_descriptor *d;
			u8 *b;
			int length;
			struct urb_frame *frame;

			d = &urb->iso_frame_desc[i];
			b = urb->transfer_buffer + d->offset;
			length = d->actual_length;

			if ((frame = kmalloc(sizeof(struct urb_frame),
					     GFP_ATOMIC))) {
				unsigned long flags;

				memcpy(frame->data, b, length);
				frame->length = length;

				spin_lock_irqsave(&dec->urb_frame_list_lock,
						     flags);
				list_add_tail(&frame->urb_frame_list,
					      &dec->urb_frame_list);
				spin_unlock_irqrestore(&dec->urb_frame_list_lock,
						       flags);

				tasklet_schedule(&dec->urb_tasklet);
			}
		}
	} else {
		 /* -ENOENT is expected when unlinking urbs */
		if (urb->status != -ENOENT)
			dprintk("%s: urb error: %d\n", __func__,
				urb->status);
	}

	if (dec->iso_stream_count)
		usb_submit_urb(urb, GFP_ATOMIC);
}