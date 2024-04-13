static rndis_resp_t *rndis_add_response(struct rndis_params *params, u32 length)
{
	rndis_resp_t *r;

	/* NOTE: this gets copied into ether.c USB_BUFSIZ bytes ... */
	r = kmalloc(sizeof(rndis_resp_t) + length, GFP_ATOMIC);
	if (!r) return NULL;

	r->buf = (u8 *)(r + 1);
	r->length = length;
	r->send = 0;

	list_add_tail(&r->list, &params->resp_queue);
	return r;
}