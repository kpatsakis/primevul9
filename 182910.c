forward_destroy(dns_forward_t *forward) {

	forward->magic = 0;
	if (forward->request != NULL)
		dns_request_destroy(&forward->request);
	if (forward->msgbuf != NULL)
		isc_buffer_free(&forward->msgbuf);
	if (forward->zone != NULL) {
		LOCK(&forward->zone->lock);
		if (ISC_LINK_LINKED(forward, link))
			ISC_LIST_UNLINK(forward->zone->forwards, forward, link);
		UNLOCK(&forward->zone->lock);
		dns_zone_idetach(&forward->zone);
	}
	isc_mem_putanddetach(&forward->mctx, forward, sizeof(*forward));
}