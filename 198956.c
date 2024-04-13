static void acm_write_buffers_free(struct acm *acm)
{
	int i;
	struct acm_wb *wb;

	for (wb = &acm->wb[0], i = 0; i < ACM_NW; i++, wb++)
		usb_free_coherent(acm->dev, acm->writesize, wb->buf, wb->dmah);
}