static void ttusb_dec_setup_urbs(struct ttusb_dec *dec)
{
	int i, j, buffer_offset = 0;

	dprintk("%s\n", __func__);

	for (i = 0; i < ISO_BUF_COUNT; i++) {
		int frame_offset = 0;
		struct urb *urb = dec->iso_urb[i];

		urb->dev = dec->udev;
		urb->context = dec;
		urb->complete = ttusb_dec_process_urb;
		urb->pipe = dec->in_pipe;
		urb->transfer_flags = URB_ISO_ASAP;
		urb->interval = 1;
		urb->number_of_packets = FRAMES_PER_ISO_BUF;
		urb->transfer_buffer_length = ISO_FRAME_SIZE *
					      FRAMES_PER_ISO_BUF;
		urb->transfer_buffer = dec->iso_buffer + buffer_offset;
		buffer_offset += ISO_FRAME_SIZE * FRAMES_PER_ISO_BUF;

		for (j = 0; j < FRAMES_PER_ISO_BUF; j++) {
			urb->iso_frame_desc[j].offset = frame_offset;
			urb->iso_frame_desc[j].length = ISO_FRAME_SIZE;
			frame_offset += ISO_FRAME_SIZE;
		}
	}
}