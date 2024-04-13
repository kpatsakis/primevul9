notify_isqueued(dns_zone_t *zone, unsigned int flags, dns_name_t *name,
		isc_sockaddr_t *addr, dns_tsigkey_t *key)
{
	dns_notify_t *notify;
	dns_zonemgr_t *zmgr;
	isc_result_t result;

	for (notify = ISC_LIST_HEAD(zone->notifies);
	     notify != NULL;
	     notify = ISC_LIST_NEXT(notify, link)) {
		if (notify->request != NULL)
			continue;
		if (name != NULL && dns_name_dynamic(&notify->ns) &&
		    dns_name_equal(name, &notify->ns))
			goto requeue;
		if (addr != NULL && isc_sockaddr_equal(addr, &notify->dst) &&
		    notify->key == key)
			goto requeue;
	}
	return (false);

requeue:
	/*
	 * If we are enqueued on the startup ratelimiter and this is
	 * not a startup notify, re-enqueue on the normal notify
	 * ratelimiter.
	 */
	if (notify->event != NULL && (flags & DNS_NOTIFY_STARTUP) == 0 &&
	    (notify->flags & DNS_NOTIFY_STARTUP) != 0) {
		zmgr = notify->zone->zmgr;
		result = isc_ratelimiter_dequeue(zmgr->startupnotifyrl,
						 notify->event);
		if (result != ISC_R_SUCCESS)
			return (true);

		notify->flags &= ~DNS_NOTIFY_STARTUP;
		result = isc_ratelimiter_enqueue(notify->zone->zmgr->notifyrl,
						 notify->zone->task,
						 &notify->event);
		if (result != ISC_R_SUCCESS) {
			isc_event_free(&notify->event);
			return (false);
		}
	}

	return (true);
}