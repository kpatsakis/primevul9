zmgr_start_xfrin_ifquota(dns_zonemgr_t *zmgr, dns_zone_t *zone) {
	dns_peer_t *peer = NULL;
	isc_netaddr_t masterip;
	uint32_t nxfrsin, nxfrsperns;
	dns_zone_t *x;
	uint32_t maxtransfersin, maxtransfersperns;
	isc_event_t *e;

	/*
	 * If we are exiting just pretend we got quota so the zone will
	 * be cleaned up in the zone's task context.
	 */
	LOCK_ZONE(zone);
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		UNLOCK_ZONE(zone);
		goto gotquota;
	}

	/*
	 * Find any configured information about the server we'd
	 * like to transfer this zone from.
	 */
	isc_netaddr_fromsockaddr(&masterip, &zone->masteraddr);
	(void)dns_peerlist_peerbyaddr(zone->view->peers, &masterip, &peer);
	UNLOCK_ZONE(zone);

	/*
	 * Determine the total maximum number of simultaneous
	 * transfers allowed, and the maximum for this specific
	 * master.
	 */
	maxtransfersin = zmgr->transfersin;
	maxtransfersperns = zmgr->transfersperns;
	if (peer != NULL)
		(void)dns_peer_gettransfers(peer, &maxtransfersperns);

	/*
	 * Count the total number of transfers that are in progress,
	 * and the number of transfers in progress from this master.
	 * We linearly scan a list of all transfers; if this turns
	 * out to be too slow, we could hash on the master address.
	 */
	nxfrsin = nxfrsperns = 0;
	for (x = ISC_LIST_HEAD(zmgr->xfrin_in_progress);
	     x != NULL;
	     x = ISC_LIST_NEXT(x, statelink))
	{
		isc_netaddr_t xip;

		LOCK_ZONE(x);
		isc_netaddr_fromsockaddr(&xip, &x->masteraddr);
		UNLOCK_ZONE(x);

		nxfrsin++;
		if (isc_netaddr_equal(&xip, &masterip))
			nxfrsperns++;
	}

	/* Enforce quota. */
	if (nxfrsin >= maxtransfersin)
		return (ISC_R_QUOTA);

	if (nxfrsperns >= maxtransfersperns)
		return (ISC_R_QUOTA);

 gotquota:
	/*
	 * We have sufficient quota.  Move the zone to the "xfrin_in_progress"
	 * list and send it an event to let it start the actual transfer in the
	 * context of its own task.
	 */
	e = isc_event_allocate(zmgr->mctx, zmgr, DNS_EVENT_ZONESTARTXFRIN,
			       got_transfer_quota, zone, sizeof(isc_event_t));
	if (e == NULL)
		return (ISC_R_NOMEMORY);

	LOCK_ZONE(zone);
	INSIST(zone->statelist == &zmgr->waiting_for_xfrin);
	ISC_LIST_UNLINK(zmgr->waiting_for_xfrin, zone, statelink);
	ISC_LIST_APPEND(zmgr->xfrin_in_progress, zone, statelink);
	zone->statelist = &zmgr->xfrin_in_progress;
	isc_task_send(zone->task, &e);
	dns_zone_log(zone, ISC_LOG_INFO, "Transfer started.");
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);
}