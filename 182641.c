notify_send(dns_notify_t *notify) {
	dns_adbaddrinfo_t *ai;
	isc_sockaddr_t dst;
	isc_result_t result;
	dns_notify_t *newnotify = NULL;
	unsigned int flags;
	bool startup;

	/*
	 * Zone lock held by caller.
	 */
	REQUIRE(DNS_NOTIFY_VALID(notify));
	REQUIRE(LOCKED_ZONE(notify->zone));

	if (DNS_ZONE_FLAG(notify->zone, DNS_ZONEFLG_EXITING))
		return;

	for (ai = ISC_LIST_HEAD(notify->find->list);
	     ai != NULL;
	     ai = ISC_LIST_NEXT(ai, publink)) {
		dst = ai->sockaddr;
		if (notify_isqueued(notify->zone, notify->flags, NULL, &dst,
				    NULL))
			continue;
		if (notify_isself(notify->zone, &dst))
			continue;
		newnotify = NULL;
		flags = notify->flags & DNS_NOTIFY_NOSOA;
		result = notify_create(notify->mctx, flags, &newnotify);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		zone_iattach(notify->zone, &newnotify->zone);
		ISC_LIST_APPEND(newnotify->zone->notifies, newnotify, link);
		newnotify->dst = dst;
		startup = ((notify->flags & DNS_NOTIFY_STARTUP) != 0);
		result = notify_send_queue(newnotify, startup);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		newnotify = NULL;
	}

 cleanup:
	if (newnotify != NULL)
		notify_destroy(newnotify, true);
}