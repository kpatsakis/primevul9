u8 *rndis_get_next_response(struct rndis_params *params, u32 *length)
{
	rndis_resp_t *r, *n;

	if (!length) return NULL;

	list_for_each_entry_safe(r, n, &params->resp_queue, list) {
		if (!r->send) {
			r->send = 1;
			*length = r->length;
			return r->buf;
		}
	}

	return NULL;
}