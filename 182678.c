notify_done(isc_task_t *task, isc_event_t *event) {
	dns_requestevent_t *revent = (dns_requestevent_t *)event;
	dns_notify_t *notify;
	isc_result_t result;
	dns_message_t *message = NULL;
	isc_buffer_t buf;
	char rcode[128];
	char addrbuf[ISC_SOCKADDR_FORMATSIZE];

	UNUSED(task);

	notify = event->ev_arg;
	REQUIRE(DNS_NOTIFY_VALID(notify));
	INSIST(task == notify->zone->task);

	isc_buffer_init(&buf, rcode, sizeof(rcode));
	isc_sockaddr_format(&notify->dst, addrbuf, sizeof(addrbuf));

	result = revent->result;
	if (result == ISC_R_SUCCESS)
		result = dns_message_create(notify->zone->mctx,
					    DNS_MESSAGE_INTENTPARSE, &message);
	if (result == ISC_R_SUCCESS)
		result = dns_request_getresponse(revent->request, message,
					DNS_MESSAGEPARSE_PRESERVEORDER);
	if (result == ISC_R_SUCCESS)
		result = dns_rcode_totext(message->rcode, &buf);
	if (result == ISC_R_SUCCESS)
		notify_log(notify->zone, ISC_LOG_DEBUG(3),
			   "notify response from %s: %.*s",
			   addrbuf, (int)buf.used, rcode);
	else
		notify_log(notify->zone, ISC_LOG_DEBUG(2),
			   "notify to %s failed: %s", addrbuf,
			   dns_result_totext(result));

	/*
	 * Old bind's return formerr if they see a soa record.	Retry w/o
	 * the soa if we see a formerr and had sent a SOA.
	 */
	isc_event_free(&event);
	if (message != NULL && message->rcode == dns_rcode_formerr &&
	    (notify->flags & DNS_NOTIFY_NOSOA) == 0) {
		bool startup;

		notify->flags |= DNS_NOTIFY_NOSOA;
		dns_request_destroy(&notify->request);
		startup = (notify->flags & DNS_NOTIFY_STARTUP);
		result = notify_send_queue(notify, startup);
		if (result != ISC_R_SUCCESS)
			notify_destroy(notify, false);
	} else {
		if (result == ISC_R_TIMEDOUT)
			notify_log(notify->zone, ISC_LOG_DEBUG(1),
				   "notify to %s: retries exceeded", addrbuf);
		notify_destroy(notify, false);
	}
	if (message != NULL)
		dns_message_destroy(&message);
}