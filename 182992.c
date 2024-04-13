dns_zone_forwardupdate(dns_zone_t *zone, dns_message_t *msg,
		       dns_updatecallback_t callback, void *callback_arg)
{
	dns_forward_t *forward;
	isc_result_t result;
	isc_region_t *mr;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(msg != NULL);
	REQUIRE(callback != NULL);

	forward = isc_mem_get(zone->mctx, sizeof(*forward));
	if (forward == NULL)
		return (ISC_R_NOMEMORY);

	forward->request = NULL;
	forward->zone = NULL;
	forward->msgbuf = NULL;
	forward->which = 0;
	forward->mctx = 0;
	forward->callback = callback;
	forward->callback_arg = callback_arg;
	ISC_LINK_INIT(forward, link);
	forward->magic = FORWARD_MAGIC;
	forward->options = DNS_REQUESTOPT_TCP;
	/*
	 * If we have a SIG(0) signed message we need to preserve the
	 * query id as that is included in the SIG(0) computation.
	 */
	if (msg->sig0 != NULL)
		forward->options |= DNS_REQUESTOPT_FIXEDID;

	mr = dns_message_getrawmessage(msg);
	if (mr == NULL) {
		result = ISC_R_UNEXPECTEDEND;
		goto cleanup;
	}

	result = isc_buffer_allocate(zone->mctx, &forward->msgbuf, mr->length);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = isc_buffer_copyregion(forward->msgbuf, mr);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	isc_mem_attach(zone->mctx, &forward->mctx);
	dns_zone_iattach(zone, &forward->zone);
	result = sendtomaster(forward);

 cleanup:
	if (result != ISC_R_SUCCESS) {
		forward_destroy(forward);
	}
	return (result);
}