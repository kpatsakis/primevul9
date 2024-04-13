void rndis_free_response(struct rndis_params *params, u8 *buf)
{
	rndis_resp_t *r, *n;

	list_for_each_entry_safe(r, n, &params->resp_queue, list) {
		if (r->buf == buf) {
			list_del(&r->list);
			kfree(r);
		}
	}
}