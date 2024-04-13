notify_send_queue(dns_notify_t *notify, bool startup) {
	isc_event_t *e;
	isc_result_t result;

	INSIST(notify->event == NULL);
	e = isc_event_allocate(notify->mctx, NULL, DNS_EVENT_NOTIFYSENDTOADDR,
			       notify_send_toaddr, notify, sizeof(isc_event_t));
	if (e == NULL)
		return (ISC_R_NOMEMORY);
	if (startup)
		notify->event = e;
	e->ev_arg = notify;
	e->ev_sender = NULL;
	result = isc_ratelimiter_enqueue(startup
					  ? notify->zone->zmgr->startupnotifyrl
					  : notify->zone->zmgr->notifyrl,
					 notify->zone->task, &e);
	if (result != ISC_R_SUCCESS) {
		isc_event_free(&e);
		notify->event = NULL;
	}
	return (result);
}