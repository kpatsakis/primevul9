static int ttusb_dec_init_usb(struct ttusb_dec *dec)
{
	int result;

	dprintk("%s\n", __func__);

	mutex_init(&dec->usb_mutex);
	mutex_init(&dec->iso_mutex);

	dec->command_pipe = usb_sndbulkpipe(dec->udev, COMMAND_PIPE);
	dec->result_pipe = usb_rcvbulkpipe(dec->udev, RESULT_PIPE);
	dec->in_pipe = usb_rcvisocpipe(dec->udev, IN_PIPE);
	dec->out_pipe = usb_sndisocpipe(dec->udev, OUT_PIPE);
	dec->irq_pipe = usb_rcvintpipe(dec->udev, IRQ_PIPE);

	if(enable_rc) {
		dec->irq_urb = usb_alloc_urb(0, GFP_KERNEL);
		if(!dec->irq_urb) {
			return -ENOMEM;
		}
		dec->irq_buffer = usb_alloc_coherent(dec->udev,IRQ_PACKET_SIZE,
					GFP_KERNEL, &dec->irq_dma_handle);
		if(!dec->irq_buffer) {
			usb_free_urb(dec->irq_urb);
			return -ENOMEM;
		}
		usb_fill_int_urb(dec->irq_urb, dec->udev,dec->irq_pipe,
				 dec->irq_buffer, IRQ_PACKET_SIZE,
				 ttusb_dec_handle_irq, dec, 1);
		dec->irq_urb->transfer_dma = dec->irq_dma_handle;
		dec->irq_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	}

	result = ttusb_dec_alloc_iso_urbs(dec);
	if (result) {
		usb_free_urb(dec->irq_urb);
		usb_free_coherent(dec->udev, IRQ_PACKET_SIZE,
				  dec->irq_buffer, dec->irq_dma_handle);
	}
	return result;
}