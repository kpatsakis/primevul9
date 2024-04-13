static void acm_read_buffers_free(struct acm *acm)
{
	int i;

	for (i = 0; i < acm->rx_buflimit; i++)
		usb_free_coherent(acm->dev, acm->readsize,
			  acm->read_buffers[i].base, acm->read_buffers[i].dma);
}